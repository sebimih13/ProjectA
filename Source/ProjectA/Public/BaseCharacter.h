// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Headers/AmmoType.h"
#include "Headers/WeaponType.h"
#include "BaseCharacter.generated.h"

/** Forward Declarations */
class USpringArmComponent;
class UCameraComponent;
class UCurveFloat;

class UBaseCharacterAnimInstance;
class ABaseCharacterPlayerController;
class AItem;
class AWeapon;
class AAmmo;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Normal			UMETA(DisplayName = "Normal"),
	Firing			UMETA(DisplayName = "Firing"),
	Reloading		UMETA(DisplayName = "Reloading")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

public:
	/** Constructors */
	FInterpLocation() {}

	FInterpLocation(USceneComponent* SceneComp)
	{
		SceneComponent = SceneComp;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent = nullptr;

	// Number of items interping to this scene component location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount = 0;
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
	void AimingButtonPressed();
	void AimingButtonReleased();

	/** Called for firing the weapon */
	void FireButtonPressed();
	void FireButtonReleased();

	/** Called for interacting with items in the world */
	void InteractButtonPressed();
	void InteractButtonReleased();

	/** Called for reloading the weapon */
	void ReloadButtonPressed();

	/** Called for displaying/removing the Inventory Wheel */
	void InventoryWheelButtonPressed();
	void InventoryWheelButtonReleased();

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

	/** Scene Component to attach to the Character's hand during reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USceneComponent* LeftHandSceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpWeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpItem1Component;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpItem2Component;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpItem3Component;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpItem4Component;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpItem5Component;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpItem6Component;

private:
	/** References */
	UBaseCharacterAnimInstance* MainAnimInstance;
	ABaseCharacterPlayerController* MainPlayerController;

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
	/** Jumping */
	FTimerHandle JumpingTimer;

	/** Crouching */
	float StandingCapsuleHalfHeight = 88.0f;
	float CrouchingCapsuleHalfHeight = 60.0f;

	/** Aiming System */
	bool bAimingButtonPressed = false;

	/** Crosshair Firing */
	bool bIsFiringBullet = false;
	float ShootTimeDuration = 0.05f;
	FTimerHandle CrosshairShootTimer;

	/** Automatic Fire */
	bool bFireButtonPressed = false;
	float AutomaticFireRate = 0.1f;
	FTimerHandle AutomaticFireTimer;

	/** Cached Variables */
	FVector PreviousVelocity = FVector::ZeroVector;
	float PreviousAimYawRate = 0.0f;

	/** Items System */
	bool bShouldTraceForItems = false;
	int8 OverlappedItemsCount = 0;
	AItem* LastTraceHitItem = nullptr;

	/** Weapon System */
	AWeapon* EquippedWeapon = nullptr;
	AItem* TraceHitItem = nullptr;

	/** Combat State of the Character */
	ECombatState CombatState = ECombatState::Normal;

	/** Items Interping Locations */
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	bool bShouldPlayPickupSound = true;

	FTimerHandle EquipSoundTimer;
	bool bShouldPlayEquipSound = true;

	/** Inventory Wheel System */
	bool bIsInventoryWheelOpen = false;

	/** Weapon Inventory */
	TMap<EWeaponType, AWeapon*> WeaponInventory;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Weapon")
	UAnimMontage* ReloadMontage;

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

	/** Time to wait before we can play another Pickup Sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Item")
	float PickupSoundResetTime = 0.2f;

	/** Time to wait before we can play another Equip Sound */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Item")
	float EquipSoundResetTime = 0.2f;

	/** Ammo System */
	TMap<EAmmoType, int32> AmmoMap;

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

	/** Aiming System */
	void StartAiming();
	void StopAiming();

	/** Performs a line trace under crosshairs */
	bool TraceUnderCrosshairs(FHitResult& HitResult);

	/** Weapon System */
	AWeapon* SpawnDefaultWeapon();					// TODO : do we need this?
	void EquipWeapon(AWeapon* WeaponToEquip);
	void DropWeapon(AWeapon* WeaponToDrop);
	void ReloadWeapon();

	/** Ammo System */
	void InitializeAmmoMap();
	bool WeaponHasAmmo();
	void PickupAmmo(AAmmo* Ammo);

	/** Check to see if we have ammo of the EquippedWeapon's ammo type */
	bool CarryingAmmo();

	/** Interps Capsule Hald Height when crouching/standing */
	void InterpCapsuleHalfHeight(float DeltaTime);

	void InitializeInterpLocations();

	/** Limit Sounds */
	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	/** Inventory Wheel System */
	void InitializeWeaponWheel();
	void AddWeaponInInventory(AWeapon* WeaponToAdd);
	EWeaponType SelectedWeaponType = EWeaponType::Unarmed;

public:
	/** Adds / Substracts to/from OverlappedItemsCount and updates bShouldTraceForItems */
	void IncrementOverlappedItemsCount(int8 Amount);

	/** Item System */
	void GetPickupItem(AItem* Item);

	void FinishReloading();

	/** Returns the index in InterpLocations[] with the lowest ItemCount */
	int32 GetInterpLocationIndex();
	void IncrementInterpLocationItemCount(int32 Index, int32 Amount);
	FVector GetInterpLocation(int32 Index);

	/** Limit Sounds */
	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; };
	FORCEINLINE USceneComponent* GetLeftHandSceneComponent() const { return LeftHandSceneComponent; };

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

	FORCEINLINE bool GetShouldPlayPickupSound() const { return bShouldPlayPickupSound; };
	FORCEINLINE bool GetShouldPlayEquipSound() const { return bShouldPlayEquipSound; };

	FORCEINLINE AWeapon* GetWeaponInInventory(EWeaponType Type) const { return WeaponInventory[Type]; };

	FORCEINLINE int32 GetTotalAmmoAmount(EAmmoType Type) const { return AmmoMap[Type]; };

	FORCEINLINE EWeaponType GetSelectedWeaponType() const { return SelectedWeaponType; };
	FORCEINLINE void SetSelectedWeaponType(EWeaponType Type) { SelectedWeaponType = Type; };
};

