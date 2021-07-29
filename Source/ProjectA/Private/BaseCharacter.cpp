// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacterAnimInstance.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f);

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
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
	
	MainAnimInstance = Cast<UBaseCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCharacterInformations(DeltaTime);

	if (MainAnimInstance->GetMovementState() == EMovementState::Grounded)
	{
		UpdateGroundedRotation(DeltaTime);

		// TODO : Move
		if (GetIsAiming())
		{
			MainAnimInstance->SetShouldSprint(false);
			CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomFOV, DeltaTime, CameraZoomInterpSpeed);
		}
		else
		{
			CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, CameraZoomInterpSpeed);
		}
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	
	// TODO
	if (MainAnimInstance->GetMovementState() == EMovementState::InAir)
	{

	}

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

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABaseCharacter::StopAiming);

	// TODO: Overlay
	PlayerInputComponent->BindAction("Default", IE_Pressed, this, &ABaseCharacter::SetDefaultOverlay);
	PlayerInputComponent->BindAction("Rifle", IE_Pressed, this, &ABaseCharacter::SetRifleOverlay);
	PlayerInputComponent->BindAction("Pistol1H", IE_Pressed, this, &ABaseCharacter::SetPistol1HOverlay);
	PlayerInputComponent->BindAction("Pistol2H", IE_Pressed, this, &ABaseCharacter::SetPistol2HOverlay);

	PlayerInputComponent->BindAction("SwitchInput", IE_Pressed, this, &ABaseCharacter::SwitchInput);
}

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

void ABaseCharacter::StartAiming()
{
	SetIsAiming(true);
}

void ABaseCharacter::StopAiming()
{
	SetIsAiming(false);
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

void ABaseCharacter::SetDefaultOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Default);
}

void ABaseCharacter::SetRifleOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Rifle);
}

void ABaseCharacter::SetPistol1HOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Pistol1H);
}

void ABaseCharacter::SetPistol2HOverlay()
{
	MainAnimInstance->SetOverlayState(EOverlayState::Pistol2H);
}

// Calculate Functions
float ABaseCharacter::CalculateGroundedRotationRate() const
{
	// TODO
	return 5.0f;
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

