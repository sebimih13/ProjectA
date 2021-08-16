// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BaseCharacterAnimInstance.h"
#include "Item.h"
#include "Weapon.h"

// TODO : debug
#include "DrawDebugHelpers.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.0f, 40.0f, 0.0f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->FieldOfView = CameraDefaultFOV;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->CrouchedHalfHeight = 60.0f;
	GetCharacterMovement()->MaxAcceleration = 1500.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 25.0f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->AirControl = 0.15f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// WeaponMesh->AttachTo(GetMesh(), FName("weapon_slot_r"));

	MainAnimInstance = Cast<UBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	EquipWeapon(SpawnDefaultWeapon());
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChangeMovementState();
	UpdateCharacterInformations(DeltaTime);

	if (MainAnimInstance->GetMovementState() == EMovementState::Grounded)
	{
		UpdateGroundedRotation(DeltaTime);
		UpdateCharacterCamera(DeltaTime);
	}
	
	// TODO
	if (MainAnimInstance->GetMovementState() == EMovementState::InAir)
	{

	}

	// Items System
	UpdateTraceForItems();

	// Cached values
	PreviousVelocity = GetVelocity();
	PreviousAimYawRate = GetControlRotation().Yaw;
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp/Down", this, &ABaseCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("LookRight/Left", this, &ABaseCharacter::TurnAtRate);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABaseCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABaseCharacter::StopSprint);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ABaseCharacter::EndJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABaseCharacter::StartCrouch);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABaseCharacter::StopAiming);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABaseCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ABaseCharacter::InteractButtonReleased);

	PlayerInputComponent->BindAction("SwitchInput", IE_Pressed, this, &ABaseCharacter::SwitchInput);

	// TODO: Overlay
	PlayerInputComponent->BindAction("Default", IE_Pressed, this, &ABaseCharacter::SetDefaultOverlay);
	PlayerInputComponent->BindAction("Rifle", IE_Pressed, this, &ABaseCharacter::SetRifleOverlay);
	PlayerInputComponent->BindAction("Pistol1H", IE_Pressed, this, &ABaseCharacter::SetPistol1HOverlay);
	PlayerInputComponent->BindAction("Pistol2H", IE_Pressed, this, &ABaseCharacter::SetPistol2HOverlay);
}

// Input Functions
void ABaseCharacter::MoveForward(float Value)
{
	const FRotator DirRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
}

void ABaseCharacter::MoveRight(float Value)
{
	const FRotator DirRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);
}

void ABaseCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(LookRightLeftRate * Rate);
}

void ABaseCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(LookUpDownRate * Rate);
}

void ABaseCharacter::StartSprint()
{
	if (InputType == EInputType::Controller)
	{
		if (MainAnimInstance->GetShouldSprint())
		{
			MainAnimInstance->SetShouldSprint(false);
		}
		else
		{
			MainAnimInstance->SetShouldSprint(true);
		}
	}
	else
	{
		MainAnimInstance->SetShouldSprint(true);
	}
}

void ABaseCharacter::StopSprint()
{
	if (InputType == EInputType::KeyboardMouse)
	{
		MainAnimInstance->SetShouldSprint(false);
	}
}

void ABaseCharacter::StartJump()
{
	MainAnimInstance->SetJumped(true);
	GetWorldTimerManager().SetTimer(JumpingTimer, this, &ABaseCharacter::ResetJump, 0.1f);
	ACharacter::Jump();
}

void ABaseCharacter::EndJump()
{
	ACharacter::StopJumping();
}

void ABaseCharacter::StartCrouch()
{
	if (MainAnimInstance->GetMovementState() == EMovementState::Grounded)
	{
		if (MainAnimInstance->GetStance() == EStance::Standing)
		{
			MainAnimInstance->SetStance(EStance::Crouching);
			Crouch();
		}
		else
		{
			MainAnimInstance->SetStance(EStance::Standing);
			UnCrouch();
		}
	}
}

void ABaseCharacter::StartAiming()
{
	SetIsAiming(true);

	// TODO : investigate
	// GetMesh()->SetRelativeRotation(FRotator(0.0f, -80.0f, 0.0f));
}

void ABaseCharacter::StopAiming()
{
	SetIsAiming(false);

	// TODO : investigate
	// GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

void ABaseCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;

	if (MainAnimInstance->GetOverlayState() == EOverlayState::Rifle)
	{
		StartFireTimer();
	}
	else
	{
		FireWeapon();
	}
}

void ABaseCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void ABaseCharacter::InteractButtonPressed()
{
	if (TraceHitItem)
	{
		AWeapon* TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
		SwapWeapon(TraceHitWeapon);
	}
}

void ABaseCharacter::InteractButtonReleased()
{

}

void ABaseCharacter::SetDefaultOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Default);
	// TODO : WeaponMesh->SetSkeletalMesh(nullptr);
}

void ABaseCharacter::SetRifleOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Rifle);
	// TODO : WeaponMesh->SetSkeletalMesh(RifleMesh);
}

void ABaseCharacter::SetPistol1HOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Pistol1H);
	// TODO : WeaponMesh->SetSkeletalMesh(PistolMesh);
}

void ABaseCharacter::SetPistol2HOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Pistol2H);
	// TODO : WeaponMesh->SetSkeletalMesh(PistolMesh);
}

void ABaseCharacter::SwitchInput(FKey Key)
{
	if (Key.IsGamepadKey())
	{
		if (InputType == EInputType::KeyboardMouse)
		{
			MainAnimInstance->SetShouldSprint(false);
		}
		SetInputType(EInputType::Controller);
	}
	else
	{
		if (InputType == EInputType::Controller)
		{
			MainAnimInstance->SetShouldSprint(false);
		}
		SetInputType(EInputType::KeyboardMouse);
	}
}

// Calculate Functions
float ABaseCharacter::CalculateGroundedRotationRate() const
{
	// TODO
	return 5.0f;
}

// Update Functions
void ABaseCharacter::UpdateGroundedRotation(float DeltaTime)
{
	if (((GetIsMoving() && GetHasMovementInput()) || GetSpeed() > 150.0f) && !HasAnyRootMotion())	// Moving
	{
		const float GroundedRotationRate = CalculateGroundedRotationRate();

		if (GetIsAiming())
		{
			SmoothCharacterRotation(FRotator(0.0f, GetControlRotation().Yaw, 0.0f), 1000.0f, 20.0f, DeltaTime);
		}
		else
		{
			float YawValue = 0.0f;
			if (MainAnimInstance->GetGait() == EGait::Sprinting)
			{
				YawValue = LastVelocityRotation.Yaw;
			}
			else
			{
				YawValue = GetControlRotation().Yaw + MainAnimInstance->GetCurveValue(FName(TEXT("YawOffset")));
			}
			SmoothCharacterRotation(FRotator(0.0f, YawValue, 0.0f), 500.0f, GroundedRotationRate, DeltaTime);
		}
	}
	else  // Not Moving
	{
		if (GetIsAiming())
		{
			LimitRotation(-100.0f, 100.0f, 20.0f, DeltaTime);
		}

		const float RotationAmountCurve = MainAnimInstance->GetCurveValue(FName(TEXT("RotationAmount")));
		if (FMath::Abs(RotationAmountCurve) > 0.001f)
		{
			AddActorWorldRotation(FRotator(0.0f, RotationAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0.0f));
			TargetRotation = GetActorRotation();
		}
	}
}

void ABaseCharacter::UpdateCharacterInformations(float DeltaTime)
{
	// Set the amount of Acceleration
	Acceleration = (GetVelocity() - PreviousVelocity) / DeltaTime;

	// Set Speed
	SetSpeed(GetVelocity().Size2D());
	SetIsMoving(Speed > 1.0f);
	if (GetIsMoving())
	{
		LastVelocityRotation = GetVelocity().ToOrientationRotator();
	}

	// Set Movement Input Amount
	MovementInputAmount = GetCharacterMovement()->GetCurrentAcceleration().Size() / GetCharacterMovement()->GetMaxAcceleration();
	SetHasMovementInput(MovementInputAmount > 0.0f);

	if (MovementInputAmount)
	{
		LastMovementInputRotation = GetCharacterMovement()->GetCurrentAcceleration().ToOrientationRotator();
	}

	// TODO : Add interp AimingRotation to current control rotation for smooth character rotation movement
	// Aim Yaw
	AimYawRate = FMath::Abs((GetControlRotation().Yaw - PreviousAimYawRate) / DeltaTime);
}

void ABaseCharacter::UpdateCharacterCamera(float DeltaTime)
{
	if (GetIsAiming())
	{
		MainAnimInstance->SetShouldSprint(false);

		GetFollowCamera()->FieldOfView = FMath::FInterpTo(GetFollowCamera()->FieldOfView, CameraZoomFOV, DeltaTime, CameraFOVInterpSpeed);

		/* TODO :
		if (EquippedWeapon && EquippedWeapon->GetItemMesh()->DoesSocketExist(FName("Aim")))
		{
			GetCameraBoom()->SetRelativeLocation(FMath::VInterpTo(GetCameraBoom()->GetRelativeLocation(), EquippedWeapon->GetItemMesh()->GetSocketTransform(FName("Aim"), ERelativeTransformSpace::RTS_Actor).GetLocation(), DeltaTime, CameraLocationInterpSpeed));
			GetFollowCamera()->FieldOfView = FMath::FInterpTo(GetFollowCamera()->FieldOfView, CameraDefaultFOV, DeltaTime, CameraFOVInterpSpeed);
		}
		else
		{
			GetCameraBoom()->SetRelativeLocation(FMath::VInterpTo(GetCameraBoom()->GetRelativeLocation(), FVector(0.0f, 0.0f, 70.0f), DeltaTime, CameraLocationInterpSpeed));
			GetFollowCamera()->FieldOfView = FMath::FInterpTo(GetFollowCamera()->FieldOfView, CameraZoomFOV, DeltaTime, CameraFOVInterpSpeed);
		}
		*/
	}
	else
	{
		// TODO : GetCameraBoom()->SetRelativeLocation(FMath::VInterpTo(GetCameraBoom()->GetRelativeLocation(), FVector(0.0f, 0.0f, 70.0f), DeltaTime, CameraLocationInterpSpeed));
		GetFollowCamera()->FieldOfView = FMath::FInterpTo(GetFollowCamera()->FieldOfView, CameraDefaultFOV, DeltaTime, CameraFOVInterpSpeed);
	}
}

void ABaseCharacter::UpdateTraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemHitResult;
		if (TraceUnderCrosshairs(ItemHitResult))
		{
			TraceHitItem = Cast<AItem>(ItemHitResult.GetActor());

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}

			if (LastTraceHitItem && LastTraceHitItem != TraceHitItem)
			{
				LastTraceHitItem->GetPickupWidget()->SetVisibility(false);
			}
			LastTraceHitItem = TraceHitItem;
		}
	}
	else if (LastTraceHitItem)
	{
		LastTraceHitItem->GetPickupWidget()->SetVisibility(false);
	}
}

// Utility Functions
void ABaseCharacter::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime)
{
	TargetRotation = FMath::RInterpConstantTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, ActorInterpSpeed));
}

void ABaseCharacter::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime)
{
	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	if (AimYawMax < Delta.Yaw || Delta.Yaw < AimYawMin)
	{
		const float TargetYaw = GetControlRotation().Yaw + (Delta.Yaw > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation(FRotator(0.0f, TargetYaw, 0.0f), 0.0f, InterpSpeed, DeltaTime);
	}
}

void ABaseCharacter::ChangeMovementState()
{
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		MainAnimInstance->SetMovementState(EMovementState::Grounded);
	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		MainAnimInstance->SetMovementState(EMovementState::InAir);
	}
}

void ABaseCharacter::StartCrosshairBulletFire()
{
	bIsFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &ABaseCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void ABaseCharacter::FinishCrosshairBulletFire()
{
	bIsFiringBullet = false;
	GetWorldTimerManager().ClearTimer(CrosshairShootTimer);
}

void ABaseCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutomaticFireTimer, this, &ABaseCharacter::AutomaticFireReset, AutomaticFireRate);
	}
}

void ABaseCharacter::AutomaticFireReset()
{
	bShouldFire = true;
	GetWorldTimerManager().ClearTimer(AutomaticFireTimer);
	
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

void ABaseCharacter::FireWeapon()
{
	if (!GetIsAiming() || MainAnimInstance->GetOverlayState() == EOverlayState::Default) return;

	// Get viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get crosshair location
	FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	FVector CrosshairWorldPosition, CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	// Perfom line trace from Crosshair location
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.0f;
		FVector BeamEndPoint = End;
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
			DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.0f, FColor::Red, false, 2.0f);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ScreenTraceHit.Location);
			BeamEndPoint = ScreenTraceHit.Location;
		}

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, EquippedWeapon->GetItemMesh()->GetSocketTransform(FName("MuzzleFlash")));
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		}
	}

	// Play weapon animation  TODO - MOVE ANIMATIONS TO WEAPON CLASS
	switch (MainAnimInstance->GetOverlayState())
	{
	case EOverlayState::Pistol1H: EquippedWeapon->GetItemMesh()->PlayAnimation(PistolFire, false); break;
	case EOverlayState::Pistol2H: EquippedWeapon->GetItemMesh()->PlayAnimation(PistolFire, false);	break;
	case EOverlayState::Rifle:	  EquippedWeapon->GetItemMesh()->PlayAnimation(RifleFire, false);	break;
	}

	StartCrosshairBulletFire();

	// TODO : Play hipfire animation
	MainAnimInstance->Montage_Play(HipFireMontage);
	MainAnimInstance->Montage_JumpToSection(FName("StartFire"));
}

void ABaseCharacter::ResetJump()
{
	MainAnimInstance->SetJumped(false);
	GetWorldTimerManager().ClearTimer(JumpingTimer);
}

bool ABaseCharacter::TraceUnderCrosshairs(FHitResult& HitResult)
{
	// Get viewport size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get crosshair location
	FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	FVector CrosshairWorldPosition, CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	// Perfom line trace from Crosshair location
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start = CrosshairWorldPosition;
		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50000.0f;
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (HitResult.bBlockingHit)
		{
			return true;
		}
	}
	return false;
}

AWeapon* ABaseCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void ABaseCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
		WeaponToEquip->AttachToComponent(GetMesh(), AttachmentRules, FName("weapon_slot_r"));

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::Equipped);
	}
}

void ABaseCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentRules);
		EquippedWeapon->SetItemState(EItemState::Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void ABaseCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	LastTraceHitItem = nullptr;
}

void ABaseCharacter::IncrementOverlappedItemsCount(int8 Amount)
{
	if (OverlappedItemsCount + Amount <= 0)
	{
		OverlappedItemsCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemsCount += Amount;
		bShouldTraceForItems = true;
	}
}

