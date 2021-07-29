// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

/** Forward Declarations */
class ABaseCharacter;
class UCurveVector;
class UCurveFloat;

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Forward		UMETA(DisplayName = "Forward"),
	Right		UMETA(DisplayName = "Right"),
	Left		UMETA(DisplayName = "Left"),
	Backward	UMETA(DisplayName = "Backward")
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walking		UMETA(DisplayName = "Walking"),
	Running		UMETA(DisplayName = "Running"),
	Sprinting	UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Grounded	UMETA(DisplayName = "Grounded"),
	InAir		UMETA(DisplayName = "InAir")
};

UENUM(BlueprintType)
enum class EOverlayState : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Rifle		UMETA(DisplayName = "Rifle"),
	Pistol1H	UMETA(DisplayName = "Pistol1H"),
	Pistol2H	UMETA(DisplayName = "Pistol2H")
};

USTRUCT(BlueprintType)
struct FVelocityBlend
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VelocityBlend")
	float F = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VelocityBlend")
	float B = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VelocityBlend")
	float L = 0.0F;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VelocityBlend")
	float R = 0.0f;
};

USTRUCT(BlueprintType)
struct FYawOffset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "YawOffset")
	float FYaw = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "YawOffset")
	float BYaw = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "YawOffset")
	float LYaw = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "YawOffset")
	float RYaw = 0.0f;
};

USTRUCT(BlueprintType)
struct FLeanAmount
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float LR = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float FB = 0.0f;
};

UCLASS()
class PROJECTA_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	/** Movement */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	FVelocityBlend VelocityBlend;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	EMovementState MovementState = EMovementState::Grounded;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	EGait Gait = EGait::Walking;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	EOverlayState OverlayState = EOverlayState::Default;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	FYawOffset YawOffset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkRunBlend = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float StrideBlend = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float StandingPlayRate = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	FVector RelativeAccelerationAmount = FVector::ZeroVector;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	FLeanAmount LeanAmount;

	/** Should Character start moving */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bShouldMove = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	bool bIsAiming = false;

	/** Rotate in place system */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	bool bRotateR = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	bool bRotateL = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	float AimSweepTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	float LeftYawTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	float RightYawTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	float ForwardYawTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|RotateInPlace")
	float RotateRate = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float BasePoseN = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float BasePoseCLF = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float SpineAdd = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float HeadAdd = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float ArmRAdd = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float ArmLAdd = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float ArmLMS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float ArmLLS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float ArmRMS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Movement|Layers")
	float ArmRLS = 0.0f;

	/** Configuration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Movement")
	float VelocityBlendInterpSpeed = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|RotateInPlace")
	float RotateMinThreshold = -50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|RotateInPlace")
	float RotateMaxThreshold = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|RotateInPlace")
	float SmoothedAimingRotationInterpSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Curves")
	UCurveVector* CurveYawOffsetFB = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Curves")
	UCurveVector* CurveYawOffsetLR = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Curves")
	UCurveFloat* CurveStrideBlendWalkN = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Curves")
	UCurveFloat* CurveStrideBlendRunN = nullptr;

private:
	/** References */
	ABaseCharacter* Character;

	/** The desired movement state of the character -> running / sprinting */
	bool bShouldSprint = false;

	/** Yaw Offset between Character Velocity and Character Base Aim */
	float YawDelta = 0.0f;

	/** Rotate in place system */
	FRotator SmoothedAimingRotation = FRotator::ZeroRotator;
	FVector2D AimingAngle = FVector2D::ZeroVector;
	FVector2D SmoothedAimingAngle = FVector2D::ZeroVector;

	// TODO: DELETE
	FRotator SpineRotation = FRotator::ZeroRotator;

private:
	/** Calculate Functions */
	/**
		This value represents the velocity amount of the actor in each direction
		(normalized so that diagonals equal 0.5 for each direction)
	*/
	FVelocityBlend CalculateVelocityBlend() const;

	/** 
		This value represents the direction the character is moving relative to the camera 
		Set the desired Gait in here based on bShouldSprint
	*/
	EMovementDirection CalculateMovementDirection();

	FVector CalculateRelativeAccelerationAmount() const;

	float CalculateWalkRunBlend();
	float CalculateStrideBlend() const;
	float CalculateStandingPlayRate() const;

	/** Utility Functions */
	bool AngleInRange(float Angle, float MinAngle, float MaxAngle) const;
	float GetAnimCurveClamped(const FName& Name, float Bias, float ClampMin, float ClampMax) const;

	/** Update Functions */
	void UpdateAimingValues(float DeltaSeconds);
	void UpdateMovementValues(float DeltaSeconds);
	void UpdateRotationValues();
	void UpdateRotateInPlaceValues();
	void UpdateLayerValues();

public:
	/** Setters and Getters */
	FORCEINLINE EGait GetGait() { return Gait; };
	void SetGait(EGait DesiredGait);

	FORCEINLINE bool GetShouldSprint() { return bShouldSprint; };
	FORCEINLINE void SetShouldSprint(bool Value) { bShouldSprint = Value; };

	FORCEINLINE EMovementState GetMovementState() { return MovementState; };
	FORCEINLINE void SetMovementState(EMovementState DesiredState) { MovementState = DesiredState; };

	FORCEINLINE EOverlayState GetOverlayState() { return OverlayState; };
	FORCEINLINE void SetOverlayState(EOverlayState DesiredState) { OverlayState = DesiredState; };
};

