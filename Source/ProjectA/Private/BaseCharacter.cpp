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
#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BaseCharacterPlayerController.h"
#include "BaseCharacterAnimInstance.h"
#include "Item.h"
#include "Weapon.h"
#include "Ammo.h"

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

	// Create Left Hand Scene Component
	LeftHandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandSceneComponent"));

	// Create Items Locations relative to the camera
	InterpWeaponComponent = CreateDefaultSubobject<USceneComponent>(TEXT("InterpWeapon"));
	InterpWeaponComponent->SetupAttachment(GetFollowCamera());
	InterpWeaponComponent->SetRelativeLocation(FVector(140.0f, -40.0f, 40.0f));

	InterpItem1Component = CreateDefaultSubobject<USceneComponent>(TEXT("InterpItem1"));
	InterpItem1Component->SetupAttachment(GetFollowCamera());
	InterpItem1Component->SetRelativeLocation(FVector(140.0f, 30.0f, -40.0f));

	InterpItem2Component = CreateDefaultSubobject<USceneComponent>(TEXT("InterpItem2"));
	InterpItem2Component->SetupAttachment(GetFollowCamera());
	InterpItem2Component->SetRelativeLocation(FVector(140.0f, -90.0f, -40.0f));

	InterpItem3Component = CreateDefaultSubobject<USceneComponent>(TEXT("InterpItem3"));
	InterpItem3Component->SetupAttachment(GetFollowCamera());
	InterpItem3Component->SetRelativeLocation(FVector(160.0f, -110.0f, -20.0f));

	InterpItem4Component = CreateDefaultSubobject<USceneComponent>(TEXT("InterpItem4"));
	InterpItem4Component->SetupAttachment(GetFollowCamera());
	InterpItem4Component->SetRelativeLocation(FVector(160.0f, 50.0f, -20.0f));

	InterpItem5Component = CreateDefaultSubobject<USceneComponent>(TEXT("InterpItem5"));
	InterpItem5Component->SetupAttachment(GetFollowCamera());
	InterpItem5Component->SetRelativeLocation(FVector(180.0f, -90.0f, 0.0f));

	InterpItem6Component = CreateDefaultSubobject<USceneComponent>(TEXT("InterpItem6"));
	InterpItem6Component->SetupAttachment(GetFollowCamera());
	InterpItem6Component->SetRelativeLocation(FVector(180.0f, 30.0f, 0.0f));

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
	
	// Set Anim Instance Reference
	MainAnimInstance = Cast<UBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	MainPlayerController = Cast<ABaseCharacterPlayerController>(GetController());

	// Configure Left Hand Scene Component
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	LeftHandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName("hand_l"));

	// Setup Camera Saturation
	FollowCamera->PostProcessSettings.bOverride_ColorSaturation = true;

	InitializeAmmoMap();
	InitializeInterpLocations();
	InitializeWeaponWheel();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ChangeMovementState();
	UpdateCharacterInformations(DeltaTime);
	InterpCapsuleHalfHeight(DeltaTime);

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

	// TODO DEBUG
	if (GetWeapon())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(10, 0, FColor::Black, GetWeapon()->GetItemName(), false);
		}
	}
	GEngine->AddOnScreenDebugMessage(30, 0, FColor::Black, CombatState == ECombatState::InInventory ? TEXT("In Inventory") : TEXT("Not in Inventory"), false);
	GEngine->AddOnScreenDebugMessage(40, 0, FColor::Black, CombatState == ECombatState::Equipping ? TEXT("Equipping") : TEXT("Not Equipping"), false);
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

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABaseCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABaseCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABaseCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ABaseCharacter::InteractButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABaseCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("InventoryWheel", IE_Pressed, this, &ABaseCharacter::InventoryWheelButtonPressed);
	PlayerInputComponent->BindAction("InventoryWheel", IE_Released, this, &ABaseCharacter::InventoryWheelButtonReleased);

	// TODO : move to player controller
	PlayerInputComponent->BindAction("SwitchInput", IE_Pressed, this, &ABaseCharacter::SwitchInput);
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
	if (GetCombatState() == ECombatState::InInventory) return;

	AddControllerYawInput(LookRightLeftRate * Rate);
}

void ABaseCharacter::LookUpAtRate(float Rate)
{
	if (GetCombatState() == ECombatState::InInventory) return;

	AddControllerPitchInput(LookUpDownRate * Rate);
}

void ABaseCharacter::StartSprint()
{
	if (MainPlayerController->GetInputType() == EInputType::Controller)
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
	if (MainPlayerController->GetInputType() == EInputType::KeyboardMouse)
	{
		MainAnimInstance->SetShouldSprint(false);
	}
}

void ABaseCharacter::StartJump()
{
	if (GetCombatState() == ECombatState::InInventory) return;

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
		}
		else
		{
			MainAnimInstance->SetStance(EStance::Standing);
		}
	}
}

void ABaseCharacter::AimingButtonPressed()
{
	if (GetCombatState() == ECombatState::InInventory) return;

	bAimingButtonPressed = true;
	if (GetCombatState() != ECombatState::Reloading)
	{
		StartAiming();
	}
}

void ABaseCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void ABaseCharacter::FireButtonPressed()
{
	if (GetCombatState() == ECombatState::InInventory) return;

	bFireButtonPressed = true;
	FireWeapon();
}

void ABaseCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void ABaseCharacter::InteractButtonPressed()
{
	if (GetCombatState() == ECombatState::InInventory) return;

	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
}

void ABaseCharacter::InteractButtonReleased()
{
	// TODO : do we need this?
}

void ABaseCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void ABaseCharacter::InventoryWheelButtonPressed()
{
	AimingButtonReleased();
	FireButtonReleased();

	MainPlayerController->DisplayInventoryWheel();
	SetCombatState(ECombatState::InInventory);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);
}

void ABaseCharacter::InventoryWheelButtonReleased()
{
	MainPlayerController->RemoveInventoryWheel();
	SetCombatState(ECombatState::Normal);

	// Get selected Weapon from Wheel Inventory
	EquipWeapon(GetWeaponInInventory(GetSelectedWeaponType()));

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void ABaseCharacter::SwitchInput(FKey Key)
{
	// TODO : move to player controller
	// check if the input have changed in other function

	if (Key.IsGamepadKey())
	{
		if (MainPlayerController->GetInputType() == EInputType::KeyboardMouse)
		{
			MainAnimInstance->SetShouldSprint(false);
		}
		MainPlayerController->SetInputType(EInputType::Controller);
	}
	else
	{
		if (MainPlayerController->GetInputType() == EInputType::Controller)
		{
			MainAnimInstance->SetShouldSprint(false);
		}
		MainPlayerController->SetInputType(EInputType::KeyboardMouse);
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
	// TODO : Update Camera Position

	// Update Camera FOV
	if (GetIsAiming())
	{
		MainAnimInstance->SetShouldSprint(false);

		GetFollowCamera()->FieldOfView = FMath::FInterpTo(GetFollowCamera()->FieldOfView, CameraZoomFOV, DeltaTime, CameraFOVInterpSpeed);

		/* TODO :
		if (GetWeapon() && GetWeapon()->GetItemMesh()->DoesSocketExist(FName("Aim")))
		{
			GetCameraBoom()->SetRelativeLocation(FMath::VInterpTo(GetCameraBoom()->GetRelativeLocation(), GetWeapon()->GetItemMesh()->GetSocketTransform(FName("Aim"), ERelativeTransformSpace::RTS_Actor).GetLocation(), DeltaTime, CameraLocationInterpSpeed));
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

	// Update Camera Global Saturation Color
	if (GetCombatState() == ECombatState::InInventory)
	{
		GetFollowCamera()->PostProcessSettings.ColorSaturation.W = FMath::FInterpTo(GetFollowCamera()->PostProcessSettings.ColorSaturation.W, 0.1f, DeltaTime, CameraSaturationSpeed);
	}
	else
	{
		GetFollowCamera()->PostProcessSettings.ColorSaturation.W = FMath::FInterpTo(GetFollowCamera()->PostProcessSettings.ColorSaturation.W, 1.0f, DeltaTime, CameraSaturationSpeed);
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

			// Show Item's Pickup Widget and enable Custom Depth
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->DisplayWidget();
				TraceHitItem->EnableCustomDepth();
			}

			if (LastTraceHitItem && LastTraceHitItem != TraceHitItem)
			{
				LastTraceHitItem->HideWidget();
				LastTraceHitItem->DisableCustomDepth();
			}
			LastTraceHitItem = TraceHitItem;
		}
	}
	else if (LastTraceHitItem)
	{
		LastTraceHitItem->HideWidget();
		LastTraceHitItem->DisableCustomDepth();
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
	SetCombatState(ECombatState::Firing);
	GetWorldTimerManager().SetTimer(AutomaticFireTimer, this, &ABaseCharacter::AutomaticFireReset, AutomaticFireRate);
}

void ABaseCharacter::AutomaticFireReset()
{
	GetWorldTimerManager().ClearTimer(AutomaticFireTimer);

	if (GetCombatState() == ECombatState::Firing)
	{
		SetCombatState(ECombatState::Normal);
	}

	if (WeaponHasAmmo() && bFireButtonPressed && GetWeapon()->GetWeaponType() == EWeaponType::AssaultRifle)
	{
		FireWeapon();
	}
}

void ABaseCharacter::FireWeapon()
{
	if (!WeaponHasAmmo() || !GetWeapon() || !GetIsAiming() || GetCombatState() != ECombatState::Normal) return;

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

	// Perfom line trace from Crosshair Location
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

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, GetWeapon()->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash")));
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		}
	}

	GetWeapon()->Fire();
	StartCrosshairBulletFire();

	// Play hipfire animation
	MainAnimInstance->Montage_Play(HipFireMontage);
	MainAnimInstance->Montage_JumpToSection(FName("StartFire"));

	// Substract 1 from the Weapon's ammo
	GetWeapon()->DecrementAmmo();

	if (!WeaponHasAmmo())
	{
		ReloadWeapon();
	}

	// Auto Firing
	StartFireTimer();
}

void ABaseCharacter::ResetJump()
{
	MainAnimInstance->SetJumped(false);
	GetWorldTimerManager().ClearTimer(JumpingTimer);
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
	// Put the current Equipped Weapon back in the Inventory
	if (GetWeapon())
	{
		GetWeapon()->SetItemState(EItemState::PickedUp);
	}

	// Equip the new Weapon
	if (WeaponToEquip)
	{
		if (WeaponToEquip != GetWeapon())
		{
			SetCombatState(ECombatState::Equipping);
			MainAnimInstance->Montage_Play(EquipMontage);
			MainAnimInstance->Montage_JumpToSection(FName("Equip"));

			WeaponToEquip->PlayEquipSound(true);
		}

		EquippedWeapon = WeaponToEquip;
		GetWeapon()->SetItemState(EItemState::Equipped);
	}
	else
	{
		EquippedWeapon = nullptr;
	}

	// Display the Ammo Widget if the character have a weapon
	if (GetSelectedWeaponType() != EWeaponType::Unarmed && GetWeapon())
	{
		MainPlayerController->DisplayHUDOverlay();
	}
	else
	{
		MainPlayerController->HideHUDOverlay();
	}

	SetAnimationOverlay();
}

void ABaseCharacter::DropWeapon(AWeapon* WeaponToDrop)
{
	FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
	WeaponToDrop->GetWeaponMesh()->DetachFromComponent(DetachmentRules);
	WeaponToDrop->SetItemState(EItemState::Falling);
	WeaponToDrop->ThrowWeapon();
}

void ABaseCharacter::ReloadWeapon()
{
	if (GetCombatState() != ECombatState::Normal || !GetWeapon()) return;

	if (CarryingAmmo() && !GetWeapon()->GetClipIsFull())
	{
		if (GetIsAiming())
		{
			StopAiming();
		}

		SetCombatState(ECombatState::Reloading);
		MainAnimInstance->Montage_Play(ReloadMontage);
		MainAnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
	}
}

void ABaseCharacter::FinishReloading()
{
	SetCombatState(ECombatState::Normal);

	if (bAimingButtonPressed)
	{
		StartAiming();
	}

	if (!GetWeapon()) return;

	const EAmmoType AmmoType = GetWeapon()->GetAmmoType();

	// Update the Ammo Map
	if (AmmoMap.Contains(AmmoType))
	{
		int32& CarriedAmmo = AmmoMap[AmmoType];
		const int32 MagEmptySpace = GetWeapon()->GetAmmoMagazineCapacity() - GetWeapon()->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			// Reload the magazine with all the ammo we have
			GetWeapon()->AddAmmo(CarriedAmmo);
			CarriedAmmo = 0;
		}
		else
		{
			// Reload the max magazine capacity
			GetWeapon()->AddAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
		}
	}
}

void ABaseCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::AssaultRifle, AssaultRifleAmmo);
	AmmoMap.Add(EAmmoType::Pistol, PistolAmmo);
}

bool ABaseCharacter::WeaponHasAmmo()
{
	if (!GetWeapon()) return false;
	return GetWeapon()->GetAmmo() > 0;
}

void ABaseCharacter::PickupAmmo(AAmmo* Ammo)
{
	AmmoMap[Ammo->GetAmmoType()] += Ammo->GetItemsCount();

	// If we don't have ammo in the weapon, try reloading
	if (GetWeapon() && GetWeapon()->GetAmmoType() == Ammo->GetAmmoType() && GetWeapon()->GetAmmo() == 0)
	{
		ReloadWeapon();
	}

	Ammo->Destroy();
}

bool ABaseCharacter::CarryingAmmo()
{
	if (!GetWeapon()) return false;

	EAmmoType AmmoType = GetWeapon()->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void ABaseCharacter::GetPickupItem(AItem* Item)
{
	AWeapon* Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		AddWeaponInInventory(Weapon);
	}

	AAmmo* Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
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

void ABaseCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight = 0.0f;
	if (MainAnimInstance->GetStance() == EStance::Standing)
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	else if (MainAnimInstance->GetStance() == EStance::Crouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}

	const float InterpHalfHeight = FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.0f);

	const float DeltaCapsuleHalfHeight = InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	GetMesh()->AddLocalOffset(FVector(0.0f, 0.0f, -DeltaCapsuleHalfHeight));

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void ABaseCharacter::InitializeInterpLocations()
{
	InterpLocations.Add(FInterpLocation(InterpWeaponComponent));

	InterpLocations.Add(FInterpLocation(InterpItem1Component));
	InterpLocations.Add(FInterpLocation(InterpItem2Component));
	InterpLocations.Add(FInterpLocation(InterpItem3Component));
	InterpLocations.Add(FInterpLocation(InterpItem4Component));
	InterpLocations.Add(FInterpLocation(InterpItem5Component));
	InterpLocations.Add(FInterpLocation(InterpItem6Component));
}

FVector ABaseCharacter::GetInterpLocation(int32 Index)
{
	return InterpLocations[Index].SceneComponent->GetComponentLocation();
}

int32 ABaseCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 0;
	int32 LowestCount = INT_MAX;
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestCount = InterpLocations[i].ItemCount;
			LowestIndex = i;
		}
	}
	return LowestIndex;
}

void ABaseCharacter::IncrementInterpLocationItemCount(int32 Index, int32 Amount)
{
	InterpLocations[Index].ItemCount += Amount;
}

void ABaseCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &ABaseCharacter::ResetPickupSoundTimer, PickupSoundResetTime);
}

void ABaseCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
	GetWorldTimerManager().ClearTimer(PickupSoundTimer);
}

void ABaseCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &ABaseCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

void ABaseCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
	GetWorldTimerManager().ClearTimer(EquipSoundTimer);
}

void ABaseCharacter::InitializeWeaponWheel()
{
	// Add 8 slots
	WeaponInventory.Add(EWeaponType::Unarmed, nullptr);
	WeaponInventory.Add(EWeaponType::AssaultRifle, nullptr);
	WeaponInventory.Add(EWeaponType::Pistol, nullptr);
	WeaponInventory.Add(EWeaponType::Shotgun, nullptr);
	WeaponInventory.Add(EWeaponType::Sniper, nullptr);
	WeaponInventory.Add(EWeaponType::GrenadeLauncher, nullptr);
	WeaponInventory.Add(EWeaponType::RocketLauncher, nullptr);
	WeaponInventory.Add(EWeaponType::SMG, nullptr);
}

void ABaseCharacter::AddWeaponInInventory(AWeapon* WeaponToAdd)
{
	// Attach it to Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	WeaponToAdd->AttachToComponent(GetMesh(), AttachmentRules, FName("weapon_slot_r"));

	TraceHitItem = nullptr;
	LastTraceHitItem = nullptr;

	WeaponToAdd->SetItemState(EItemState::PickedUp);

	if (!GetWeaponInInventory(WeaponToAdd->GetWeaponType()))	// We don't have this type of weapon in Inventory
	{
		WeaponInventory[WeaponToAdd->GetWeaponType()] = WeaponToAdd;

		// If we want this type of weapon => equip it
		if (GetSelectedWeaponType() == WeaponToAdd->GetWeaponType())
		{
			EquipWeapon(WeaponToAdd);
		}
	}
	else	// We already have this type of weapon in the Inventory => Swap it
	{
		// If we have equipped a weapon with the same EWeaponType with the new weapon => Equip the new Weapon
		if (GetWeapon() && GetWeapon()->GetWeaponType() == WeaponToAdd->GetWeaponType())
		{
			EquipWeapon(WeaponToAdd);
		}

		DropWeapon(GetWeaponInInventory(WeaponToAdd->GetWeaponType()));
		WeaponInventory[WeaponToAdd->GetWeaponType()] = WeaponToAdd;
	}
}

void ABaseCharacter::SetAnimationOverlay()
{
	if (!MainAnimInstance) return;

	if (!GetWeapon())
	{
		MainAnimInstance->SetOverlayState(EOverlayState::Default);
		return;
	}

	switch (GetWeapon()->GetWeaponType())
	{
	case EWeaponType::AssaultRifle:		MainAnimInstance->SetOverlayState(EOverlayState::Rifle);	break;
	case EWeaponType::Sniper:			MainAnimInstance->SetOverlayState(EOverlayState::Rifle);	break;
	case EWeaponType::GrenadeLauncher:	MainAnimInstance->SetOverlayState(EOverlayState::Rifle);	break;
	case EWeaponType::Shotgun:			MainAnimInstance->SetOverlayState(EOverlayState::Rifle);	break;
	case EWeaponType::Pistol:			MainAnimInstance->SetOverlayState(EOverlayState::Pistol1H);	break;

	// TODO
	case EWeaponType::RocketLauncher:	MainAnimInstance->SetOverlayState(EOverlayState::Rifle);	break;
	case EWeaponType::SMG:				MainAnimInstance->SetOverlayState(EOverlayState::Pistol2H);	break;
	}

	// TODO : FABRIK
	// TODO : Rotate Grenade Launcher Clip
	// TODO : Shotgun : Loop reload clip
}

