// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/** Forward Declarations */
class USpringArmComponent;
class UCameraComponent;
class UBaseCharacterAnimInstance;

/** TODO : Put this in character controller */
UENUM(BlueprintType)
enum class EInputType : uint8
{
	Controller		UMETA(DisplayName = "Controller"),
	KeyboardMouse	UMETA(DisplayName = "KeyboardMouse")
};

UCLASS()
class PROJECTA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called via input to turn at a given rate */
	void TurnAtRate(float Rate);

	/** Called via input to turn look up/down at a given rate */
	void LookUpAtRate(float Rate);

	/** Called for sprinting */
	void StartSprint();
	void StopSprint();

	/** Called for jumping */
	void StartJump();
	void EndJump();

	/** Called for crouching */
	void StartCrouch();

	/** Called for aiming */
	void StartAiming();
	void StopAiming();

	/** Called for firing the weapon */
	void FireButtonPressed();
	void FireButtonReleased();

	/** Switch Weapons */
	void SetDefaultOverlay();
	void SetRifleOverlay();
	void SetPistol1HOverlay();
	void SetPistol2HOverlay();

	/** Called for switching inputs */
	void SwitchInput(FKey Key);

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Skeletal Mesh Component for weapon slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

private:
	/** References */
	UBaseCharacterAnimInstance* MainAnimInstance;

	/** Input Type */
	EInputType InputType = EInputType::KeyboardMouse;

	/** Character Informations */
	FVector Acceleration = FVector::ZeroVector;

	float Speed = 0.0f;
	bool bIsMoving = false;
	FRotator LastVelocityRotation = FRotator::ZeroRotator;

	float MovementInputAmount = 0.0f;
	bool bHasMovementInput = false;
	FRotator LastMovementInputRotation = FRotator::ZeroRotator;

	float AimYawRate = 0.0f;

	/** Rotation System */
	FRotator TargetRotation = FRotator::ZeroRotator;

	/** Is character aiming / is Aim Button pressed */
	bool bIsAiming = false;

	/** Crosshair Firing */
	bool bIsFiringBullet = false;
	float ShootTimeDuration = 0.05f;
	FTimerHandle CrosshairShootTimer;

	/** Automatic Fire */
	bool bFireButtonPressed = false;
	bool bShouldFire = true;
	float AutomaticFireRate = 0.1f;
	FTimerHandle AutomaticFireTimer;

	/** Cached Variables */
	FVector PreviousVelocity = FVector::ZeroVector;
	float PreviousAimYawRate = 0.0f;

public:
	/** Base turn rate, in deg/sec */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Camera")
	float LookRightLeftRate = 1.25f;

	/** Base look up/down rate, in deg/sec */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Camera")
	float LookUpDownRate = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Camera")
	float CameraZoomFOV = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Camera")
	float CameraDefaultFOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Camera")
	float CameraFOVInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Camera")
	float CameraLocationInterpSpeed = 20.0f;

	/** Movement */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Movement")
	float WalkSpeed = 175.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Movement")
	float RunSpeed = 375.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Movement")
	float SprintSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Movement")
	float CrouchSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	USkeletalMesh* PistolMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	USkeletalMesh* RifleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	UAnimationAsset* PistolFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	UAnimationAsset* RifleFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	UAnimMontage* HipFireMontage;

	/** Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	UParticleSystem* ImpactParticles;

	/** Trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapons")
	UParticleSystem* BeamParticles;

private:
	/** Calculate Functions */
	float CalculateGroundedRotationRate() const;

	/** Update Functions */
	void UpdateGroundedRotation(float DeltaTime);
	void UpdateCharacterInformations(float DeltaTime);
	void UpdateCharacterCamera(float DeltaTime);

	/** Utility Functions */
	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);
	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);

	/** Sets the bFiringBullet to true/false */
	void StartCrosshairBulletFire();
	void FinishCrosshairBulletFire();

	/** Fire the weapon */
	void StartFireTimer();
	void AutomaticFireReset();
	void FireWeapon();

public:
	/** FORCEINLINE Setters / Getters */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	FORCEINLINE EInputType GetInputType() const { return InputType; };
	FORCEINLINE void SetInputType(EInputType Type) { InputType = Type; };

	FORCEINLINE bool GetIsMoving() const { return bIsMoving; };
	FORCEINLINE void SetIsMoving(bool State) { bIsMoving = State; };

	FORCEINLINE bool GetHasMovementInput() const { return bHasMovementInput; };
	FORCEINLINE void SetHasMovementInput(bool State) { bHasMovementInput = State; };

	FORCEINLINE float GetSpeed() const { return Speed; };
	FORCEINLINE void SetSpeed(float Value) { Speed = Value; };

	FORCEINLINE FVector GetAcceleration() const { return Acceleration; };

	FORCEINLINE bool GetIsAiming() const { return bIsAiming; };
	FORCEINLINE void SetIsAiming(bool State) { bIsAiming = State; };

	FORCEINLINE bool GetIsFiringBullet() const { return bIsFiringBullet; };

	FORCEINLINE float GetAimYawRate() const { return AimYawRate; };
};

