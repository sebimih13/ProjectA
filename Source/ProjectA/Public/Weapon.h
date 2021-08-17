// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime = 0.7f;
	bool bFalling = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 Ammo = 0;

private:
	void StopFalling();

public:
	/** Adds an impulse to the Weapon */
	void ThrowWeapon();

	void DecrementAmmo();

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE int32 GetAmmo() const { return Ammo; };
};

