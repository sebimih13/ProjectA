// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

AWeapon::AWeapon()
{

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetItemState() == EItemState::Falling && bFalling)
	{
		const FRotator MeshRotation = FRotator(0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f);
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	const FRotator MeshRotation = FRotator(0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f);
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward = GetItemMesh()->GetForwardVector();
	const FVector MeshRight = GetItemMesh()->GetRightVector();

	// Direction in which we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.0f, MeshForward);

	const float RandomRotation = 30.0f;	// todo : FMath::FRandRange(10.0f, 30.0f);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.0f, 0.0f, 1.0f));
	ImpulseDirection *= 2'000.0f;

	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
	GetWorldTimerManager().ClearTimer(ThrowWeaponTimer);
	bFalling = false;
	SetItemState(EItemState::Pickup);
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		Ammo--;
	}
}

