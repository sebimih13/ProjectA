// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/** Forward Declarations */


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AssaultRifle		UMETA(DisplayName = "AssaultRifle"),
	Pistol				UMETA(DisplayName = "Pistol"),
	Shotgun				UMETA(DisplayName = "Shotgun"),
	Sniper				UMETA(DisplayName = "Sniper"),
	GrenadeLauncher		UMETA(DisplayName = "GrenadeLauncher"),
	RocketLauncher		UMETA(DisplayName = "RocketLauncher")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	AssaultRifle	UMETA(DisplayName = "AssaultRifle"),
	Pistol			UMETA(DisplayName = "Pistol")
};

UCLASS()
class PROJECTA_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime = 0.7f;
	bool bFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::AssaultRifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType = EAmmoType::AssaultRifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 AmmoMagazineCapacity = 32;

	FName ReloadMontageSection = FName("ReloadRifle");

	void StopFalling();

public:
	/** Adds an impulse to the Weapon */
	void ThrowWeapon();

	void DecrementAmmo();

	void AddAmmo(int32 Amount);

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };
	FORCEINLINE int32 GetAmmo() const { return Ammo; };
	FORCEINLINE int32 GetAmmoMagazineCapacity() const { return AmmoMagazineCapacity; };
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; };
};

