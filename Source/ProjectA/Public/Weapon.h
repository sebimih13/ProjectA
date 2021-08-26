// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Headers/AmmoType.h"
#include "Headers/WeaponType.h"
#include "Weapon.generated.h"

/** Forward Declarations */


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

	virtual void SetItemProperties() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* WeaponIconTexture = nullptr;

	FName ReloadMontageSection = FName("ReloadRifle");

private:
	void StopFalling();

public:
	/** Adds an impulse to the Weapon */
	void ThrowWeapon();

	void DecrementAmmo();

	void AddAmmo(int32 Amount);

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; };

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };
	FORCEINLINE int32 GetAmmo() const { return Ammo; };
	FORCEINLINE int32 GetAmmoMagazineCapacity() const { return AmmoMagazineCapacity; };
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; };
	FORCEINLINE UTexture2D* GetAmmoIconTexture() const { return AmmoIconTexture; };
	FORCEINLINE UTexture2D* GetWeaponIconTexture() const { return WeaponIconTexture; };

	FORCEINLINE bool GetClipIsFull() const { return Ammo == AmmoMagazineCapacity; };
};

