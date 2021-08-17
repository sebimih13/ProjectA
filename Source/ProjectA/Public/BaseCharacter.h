// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/** Forward Declarations */
class USpringArmComponent;
class UCameraComponent;
class UBaseCharacterAnimInstance;
class UCurveFloat;
class AItem;
class AWeapon;

/** TODO : Put this in character controller */
UENUM(BlueprintType)
enum class EInputType : uint8
{
	Controller		UMETA(DisplayName = "Controller"),
	KeyboardMouse	UMETA(DisplayName = "KeyboardMouse")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	AssaultRifle	UMETA(DisplayName = "AssaultRifle"),
	Pistol			UMETA(DisplayName = "Pistol")
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

	/** Called for interacting with items in the world */
	void InteractButtonPressed();
	void InteractButtonReleased();

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

	/** Jumping */
	FTimerHandle JumpingTimer;

	/** Cached Variables */
	FVector PreviousVelocity = FVector::ZeroVector;
	float PreviousAimYawRate = 0.0f;

	/** Items System */
	bool bShouldTraceForItems = false;
	int8 OverlappedItemsCount = 0;
	AItem* LastTraceHitItem = nullptr;

	/** Weapon System */
	AWeapon* EquippedWeapon;
	AItem* TraceHitItem;

	/** Ammo System */
	TMap<EAmmoType, int32> AmmoMap;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	UAnimationAsset* PistolFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	UAnimationAsset* RifleFire;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	UAnimMontage* HipFireMontage;

	/** Particles spawned upon bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	UParticleSystem* ImpactParticles;

	/** Trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	UParticleSystem* BeamParticles;

	/** Default Weapon to spawn with Character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Weapon")
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** Distance outward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	float CameraInterpDistance = 200.0f;

	/** Distance upward from the camera for the interp destination */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	float CameraInterpElevation = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	int32 AssaultRifleAmmo = 24;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	int32 PistolAmmo = 12;

private:
	/** Calculate Functions */
	float CalculateGroundedRotationRate() const;

	/** Update Functions */
	void UpdateGroundedRotation(float DeltaTime);
	void UpdateCharacterInformations(float DeltaTime);
	void UpdateCharacterCamera(float DeltaTime);
	void UpdateTraceForItems();

	/** Utility Functions */
	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);
	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);
	void ChangeMovementState();

	/** Sets the bFiringBullet to true/false */
	void StartCrosshairBulletFire();
	void FinishCrosshairBulletFire();

	/** Fire the weapon */
	void StartFireTimer();
	void AutomaticFireReset();
	void FireWeapon();

	/** Jumping */
	void ResetJump();

	/** Performs a line trace under crosshairs */
	bool TraceUnderCrosshairs(FHitResult& HitResult);

	/** Weapon System */
	AWeapon* SpawnDefaultWeapon();
	void EquipWeapon(AWeapon* WeaponToEquip);
	void DropWeapon();
	void SwapWeapon(AWeapon* WeaponToSwap);

	/** Ammo System */
	void InitializeAmmoMap();

	/** Check if the weapon has ammo */
	bool WeaponHasAmmo();

public:
	/** Adds / Substracts to/from OverlappedItemsCount and updates bShouldTraceForItems */
	void IncrementOverlappedItemsCount(int8 Amount);

	/** Item System */
	void GetPickupItem(AItem* Item);
	FVector GetCameraInterpLocation() const;

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

	FORCEINLINE int8 GetOverlappedItemsCount() const { return OverlappedItemsCount; };

	FORCEINLINE AWeapon* GetWeapon() const { return EquippedWeapon; };
};

