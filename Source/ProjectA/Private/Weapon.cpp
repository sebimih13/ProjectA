// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "PickupWidget.h"

AWeapon::AWeapon()
{
	// Create the Weapon Mesh Component
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// Attach other components to Weapon Mesh
	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Pickup Widget Reference to this class
	UPickupWidget* PickupWidget = Cast<UPickupWidget>(GetPickupWidget()->GetUserWidgetObject());
	if (PickupWidget)
	{
		PickupWidget->SetWeaponReference(this);
	}

	switch (WeaponType)
	{
	case EWeaponType::AssaultRifle:		ReloadMontageSection = FName("ReloadRifle");			break;
	case EWeaponType::Pistol:			ReloadMontageSection = FName("ReloadPistol");			break;
	case EWeaponType::Shotgun:			ReloadMontageSection = FName("ReloadShotgun");			break;
	case EWeaponType::Sniper:			ReloadMontageSection = FName("ReloadSniper");			break;
	case EWeaponType::GrenadeLauncher:	ReloadMontageSection = FName("ReloadGrenadeLauncher");	break;
	case EWeaponType::RocketLauncher:	ReloadMontageSection = FName("ReloadRocketLauncher");	break;
	case EWeaponType::SMG:				ReloadMontageSection = FName("ReloadSMG");				break;
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetItemState() == EItemState::Falling && bFalling)
	{
		const FRotator MeshRotation = FRotator(0.0f, WeaponMesh->GetComponentRotation().Yaw, 0.0f);
		WeaponMesh->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	WeaponMesh->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
}

void AWeapon::SetItemProperties()
{
	Super::SetItemProperties();

	switch (GetItemState())
	{
	case EItemState::Pickup:
		WeaponMesh->SetVisibility(true);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::Equipped:
		WeaponMesh->SetVisibility(true);
		UE_LOG(LogTemp, Warning, TEXT("SET VISIBILITY TRUE"));

		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::Falling:
		WeaponMesh->SetVisibility(true);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EquipInterping:
		WeaponMesh->SetVisibility(true);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::PickedUp:
		WeaponMesh->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("SET VISIBILITY FALSE"));

		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AWeapon::ThrowWeapon()
{
	const FRotator MeshRotation = FRotator(0.0f, WeaponMesh->GetComponentRotation().Yaw, 0.0f);
	WeaponMesh->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward = WeaponMesh->GetForwardVector();
	const FVector MeshRight = WeaponMesh->GetRightVector();

	// Direction in which we throw the weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.0f, MeshForward);

	const float RandomRotation = 30.0f;	// todo : FMath::FRandRange(10.0f, 30.0f);
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.0f, 0.0f, 1.0f));
	ImpulseDirection *= 2'000.0f;
	WeaponMesh->AddImpulse(ImpulseDirection);

	EnableGlowMaterial();

	bFalling = true;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
	bFalling = false;
	GetWorldTimerManager().ClearTimer(ThrowWeaponTimer);
	SetItemState(EItemState::Pickup);
	StartPulseTimer();
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

void AWeapon::AddAmmo(int32 Amount)
{
	Ammo += Amount;
}

void AWeapon::EnableCustomDepth()
{
	Super::EnableCustomDepth();

	if (GetCanChangeCustomDepth())
	{
		WeaponMesh->SetRenderCustomDepth(true);
	}
}

void AWeapon::DisableCustomDepth()
{
	Super::DisableCustomDepth();

	if (GetCanChangeCustomDepth())
	{
		WeaponMesh->SetRenderCustomDepth(false);
	}
}

