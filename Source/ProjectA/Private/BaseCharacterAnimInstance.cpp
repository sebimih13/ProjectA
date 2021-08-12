// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterAnimInstance.h"
#include "BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"

UBaseCharacterAnimInstance::UBaseCharacterAnimInstance()
{

}

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Don't run in editor
	if (!Character || DeltaSeconds == 0.0f)
	{
		return;
	}

	UpdateAimingValues(DeltaSeconds);
	UpdateLayerValues();

	if (GetMovementState() == EMovementState::Grounded)
	{
		const bool bPreviousShouldMove = bShouldMove;
		bShouldMove = (Character->GetHasMovementInput() && Character->GetIsMoving()) || Character->GetSpeed() > 150.0f;

		if (!bPreviousShouldMove && bShouldMove)	// do when starting to move
		{
			bRotateR = false;
			bRotateL = false;
		}

		if (bShouldMove)	// do while moving
		{
			UpdateMovementValues(DeltaSeconds);
			UpdateRotationValues();
		}
		else	// do while not moving
		{
			// Rotate in place system
			if (Character->GetIsAiming())
			{
				UpdateRotateInPlaceValues();
			}
			else
			{
				bRotateR = false;
				bRotateL = false;
			}
		}
	}

	// TODO
	if (GetMovementState() == EMovementState::InAir)
	{

	}
}

// Calculate Functions
FVelocityBlend UBaseCharacterAnimInstance::CalculateVelocityBlend() const
{
	const FVector LocRelativeVelocityDir = Character->GetActorRotation().UnrotateVector(Character->GetVelocity().GetSafeNormal(0.1f));
	const float Sum = FMath::Abs(LocRelativeVelocityDir.X) + FMath::Abs(LocRelativeVelocityDir.Y) + FMath::Abs(LocRelativeVelocityDir.Z);
	const FVector RelativeDir = LocRelativeVelocityDir / Sum;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Black, FString::Printf(TEXT("LOC : %f  %f  %f"), LocRelativeVelocityDir.X, LocRelativeVelocityDir.Y, LocRelativeVelocityDir.Z));
		GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Black, FString::Printf(TEXT("ROT : %f  %f  %f"), Character->GetActorRotation().Pitch, Character->GetActorRotation().Yaw, Character->GetActorRotation().Roll));
		GEngine->AddOnScreenDebugMessage(3, 0.0f, FColor::Black, FString::Printf(TEXT("VEL : %f  %f  %f"), Character->GetVelocity().X, Character->GetVelocity().Y, Character->GetVelocity().Z));
		GEngine->AddOnScreenDebugMessage(4, 0.0f, FColor::Black, FString::Printf(TEXT("DIR : %f  %f  %f"), RelativeDir.X, RelativeDir.Y, RelativeDir.Z));
	}

	FVelocityBlend Result;
	Result.F = FMath::Clamp(RelativeDir.X, 0.0f, 1.0f);
	Result.B = FMath::Abs(FMath::Clamp(RelativeDir.X, -1.0f, 0.0f));
	Result.L = FMath::Abs(FMath::Clamp(RelativeDir.Y, -1.0f, 0.0f));
	Result.R = FMath::Clamp(RelativeDir.Y, 0.0f, 1.0f);
	return Result;
}

EMovementDirection UBaseCharacterAnimInstance::CalculateMovementDirection()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(15, 0.0f, FColor::Black, FString::Printf(TEXT("YawDelta : %f"), YawDelta));
	}

	if (AngleInRange(YawDelta, -70.0f, 70.0f))
	{
		GetShouldSprint() ? SetGait(EGait::Sprinting) : SetGait(EGait::Running);
		return EMovementDirection::Forward;
	}

	SetGait(EGait::Running);
	if (AngleInRange(YawDelta, 70.0f, 110.f))
	{
		return EMovementDirection::Right;
	}

	if (AngleInRange(YawDelta, -110.0f, -70.0f))
	{
		return EMovementDirection::Left;
	}
	return EMovementDirection::Backward;
}

FVector UBaseCharacterAnimInstance::CalculateRelativeAccelerationAmount() const
{
	if (FVector::DotProduct(Character->GetAcceleration(), Character->GetVelocity()) > 0.0f)
	{
		const float MaxAcc = Character->GetCharacterMovement()->GetMaxAcceleration();
		return Character->GetActorRotation().UnrotateVector(Character->GetAcceleration().GetClampedToMaxSize(MaxAcc) / MaxAcc);
	}

	const float MaxBrakingDec = Character->GetCharacterMovement()->GetMaxBrakingDeceleration();
	return Character->GetActorRotation().UnrotateVector(Character->GetAcceleration().GetClampedToMaxSize(MaxBrakingDec) / MaxBrakingDec);
}

float UBaseCharacterAnimInstance::CalculateWalkRunBlend() const
{
	return (GetGait() == EGait::Walking) ? 0.0f : 1.0f;
}

float UBaseCharacterAnimInstance::CalculateStrideBlend() const
{
	const float CurveTime = Character->GetSpeed() / GetOwningComponent()->GetComponentScale().Z;
	const float ClampedGait = GetAnimCurveClamped(FName(TEXT("Weight_Gait")), -1.0f, 0.0f, 1.0f);
	const float LerpedStrideBlend = FMath::Lerp(CurveStrideBlendWalkN->GetFloatValue(CurveTime), CurveStrideBlendRunN->GetFloatValue(CurveTime), ClampedGait);
	return FMath::Lerp(LerpedStrideBlend, CurveStrideBlendWalkC->GetFloatValue(Character->GetSpeed()), BasePoseCLF);
}

float UBaseCharacterAnimInstance::CalculateStandingPlayRate() const
{
	const float LerpedSpeed = FMath::Lerp(Character->GetSpeed() / Character->WalkSpeed, Character->GetSpeed() / Character->RunSpeed, GetAnimCurveClamped(FName(TEXT("Weight_Gait")), -1.0f, 0.0f, 1.0f));
	const float LerpedSprint = FMath::Lerp(LerpedSpeed, Character->GetSpeed() / Character->SprintSpeed, GetAnimCurveClamped(FName(TEXT("Weight_Gait")), -2.0f, 0.0f, 1.0f));
	return FMath::Clamp(LerpedSprint / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.0f, 3.0f);
}

float UBaseCharacterAnimInstance::CalculateCrouchingPlayRate() const
{
	return FMath::Clamp(Character->GetSpeed() / Character->CrouchSpeed / StrideBlend / GetOwningComponent()->GetComponentScale().Z, 0.0f, 2.0f);
}

// Utility Functions
bool UBaseCharacterAnimInstance::AngleInRange(float Angle, float MinAngle, float MaxAngle) const
{
	return MinAngle <= Angle && Angle <= MaxAngle;
}

float UBaseCharacterAnimInstance::GetAnimCurveClamped(const FName& Name, float Bias, float ClampMin, float ClampMax) const
{
	return FMath::Clamp(GetCurveValue(Name) + Bias, ClampMin, ClampMax);
}

// Setters
void UBaseCharacterAnimInstance::SetGait(EGait DesiredGait)
{
	Gait = DesiredGait;
	switch (DesiredGait)
	{
	case EGait::Walking:   Character->GetCharacterMovement()->MaxWalkSpeed = Character->WalkSpeed;   break;
	case EGait::Running:   Character->GetCharacterMovement()->MaxWalkSpeed = Character->RunSpeed;	 break;
	case EGait::Sprinting: Character->GetCharacterMovement()->MaxWalkSpeed = Character->SprintSpeed; break;
	}
}

// Update Functions
void UBaseCharacterAnimInstance::UpdateAimingValues(float DeltaSeconds)
{
	bIsAiming = Character->GetIsAiming();

	SmoothedAimingRotation = FMath::RInterpTo(SmoothedAimingRotation, Character->GetControlRotation(), DeltaSeconds, SmoothedAimingRotationInterpSpeed);

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetControlRotation(), Character->GetActorRotation());
	AimingAngle.X = Delta.Yaw;
	AimingAngle.Y = Delta.Pitch;

	Delta = UKismetMathLibrary::NormalizedDeltaRotator(SmoothedAimingRotation, Character->GetActorRotation());
	SmoothedAimingAngle.X = Delta.Yaw;
	SmoothedAimingAngle.Y = Delta.Pitch;

	AimSweepTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.0f, 90.0f), FVector2D(1.0f, 0.0f), AimingAngle.Y);

	SpineRotation.Roll = 0.0f;
	SpineRotation.Pitch = 0.0f;
	SpineRotation.Yaw = AimingAngle.X / 4.0f;

	HeadYawTime = FMath::GetMappedRangeValueClamped(FVector2D(-90.0f, 90.0f), FVector2D(0.0f, 1.0f), SmoothedAimingAngle.X);
}

void UBaseCharacterAnimInstance::UpdateMovementValues(float DeltaSeconds)
{
	// Calculate Yaw Delta
	YawDelta = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetVelocity().ToOrientationRotator(), Character->GetBaseAimRotation()).Yaw;

	// Interp and set Velocity Blend
	const FVelocityBlend& VelocityBlendTarget = CalculateVelocityBlend();
	VelocityBlend.F = FMath::FInterpTo(VelocityBlend.F, VelocityBlendTarget.F, DeltaSeconds, VelocityBlendInterpSpeed);
	VelocityBlend.B = FMath::FInterpTo(VelocityBlend.B, VelocityBlendTarget.B, DeltaSeconds, VelocityBlendInterpSpeed);
	VelocityBlend.L = FMath::FInterpTo(VelocityBlend.L, VelocityBlendTarget.L, DeltaSeconds, VelocityBlendInterpSpeed);
	VelocityBlend.R = FMath::FInterpTo(VelocityBlend.R, VelocityBlendTarget.R, DeltaSeconds, VelocityBlendInterpSpeed);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(10, 0.0f, FColor::Black, FString::Printf(TEXT("F : %f"), VelocityBlend.F));
		GEngine->AddOnScreenDebugMessage(11, 0.0f, FColor::Black, FString::Printf(TEXT("B : %f"), VelocityBlend.B));
		GEngine->AddOnScreenDebugMessage(12, 0.0f, FColor::Black, FString::Printf(TEXT("L : %f"), VelocityBlend.L));
		GEngine->AddOnScreenDebugMessage(13, 0.0f, FColor::Black, FString::Printf(TEXT("R : %f"), VelocityBlend.R));
	}

	// Set the Relative Acceleration Amount and Interp the Lean Amount
	RelativeAccelerationAmount = CalculateRelativeAccelerationAmount();
	LeanAmount.LR = FMath::FInterpTo(LeanAmount.LR, RelativeAccelerationAmount.Y, DeltaSeconds, 4.0f);
	LeanAmount.FB = FMath::FInterpTo(LeanAmount.LR, RelativeAccelerationAmount.X, DeltaSeconds, 4.0f);

	// Set WalkRunBlend / StrideBlend / StandingPlayRate
	WalkRunBlend = CalculateWalkRunBlend();
	StrideBlend = CalculateStrideBlend();
	StandingPlayRate = CalculateStandingPlayRate();
	CrouchingPlayRate = CalculateCrouchingPlayRate();
}

void UBaseCharacterAnimInstance::UpdateRotationValues()
{
	// Calculate Movement Direction
	MovementDirection = CalculateMovementDirection();

	// Yaw Offsets
	const FVector& FBOffset = CurveYawOffsetFB->GetVectorValue(YawDelta);
	YawOffset.FYaw = FBOffset.X;
	YawOffset.BYaw = FBOffset.Y;

	const FVector& LROffset = CurveYawOffsetLR->GetVectorValue(YawDelta);
	YawOffset.LYaw = LROffset.X;
	YawOffset.RYaw = LROffset.Y;
}

void UBaseCharacterAnimInstance::UpdateRotateInPlaceValues()
{
	bRotateL = AimingAngle.X < RotateMinThreshold;
	bRotateR = AimingAngle.X > RotateMaxThreshold;

	if (bRotateL || bRotateR)
	{
		RotateRate = FMath::GetMappedRangeValueClamped(FVector2D(90.0f, 270.0f), FVector2D(1.15f, 3.0f), Character->GetAimYawRate());
	}
}

void UBaseCharacterAnimInstance::UpdateLayerValues()
{
	BasePoseN = GetCurveValue(FName(TEXT("BasePose_N")));
	BasePoseCLF = GetCurveValue(FName(TEXT("BasePose_CLF")));

	ArmRAdd = GetCurveValue(FName(TEXT("Layering_Arm_R_Add")));
	ArmLAdd = GetCurveValue(FName(TEXT("Layering_Arm_L_Add")));
	SpineAdd = GetCurveValue(FName(TEXT("Layering_Spine_Add")));
	HeadAdd = GetCurveValue(FName(TEXT("Layering_Head_Add")));

	ArmLLS = GetCurveValue(FName(TEXT("Layering_Arm_L_LS")));
	ArmLMS = static_cast<float>(1 - FMath::FloorToInt(ArmLLS));

	ArmRLS = GetCurveValue(FName(TEXT("Layering_Arm_R_LS")));
	ArmRMS = static_cast<float>(1 - FMath::FloorToInt(ArmRLS));
}

