// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "PickupWidget.h"
#include "SwitchWeaponWidget.h"
#include "BaseCharacter.h"

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

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemRarityDataTable)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity)
		{
		case EItemRarity::Common:	 RarityRow = ItemRarityDataTable->FindRow<FItemRarityTable>(FName("Common"),	 TEXT(""));	break;
		case EItemRarity::Uncommon:  RarityRow = ItemRarityDataTable->FindRow<FItemRarityTable>(FName("Uncommon"),   TEXT(""));	break;
		case EItemRarity::Rare:		 RarityRow = ItemRarityDataTable->FindRow<FItemRarityTable>(FName("Rare"),		 TEXT(""));	break;
		case EItemRarity::Epic:		 RarityRow = ItemRarityDataTable->FindRow<FItemRarityTable>(FName("Epic"),		 TEXT(""));	break;
		case EItemRarity::Legendary: RarityRow = ItemRarityDataTable->FindRow<FItemRarityTable>(FName("Legendary"),  TEXT(""));	break;
		}

		if (RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColorBackground = RarityRow->LightColorBackground;
			DarkColorBackground = RarityRow->DarkColorBackground;
			NumberOfStars = RarityRow->NumberOfStars;
			WeaponMesh->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
		}
	}

	if (WeaponDataTable)
	{
		FWeaponDataTable* WeaponRow = nullptr;
		switch (Weapon)
		{
		// WeaponType == AssaultRifle
		case EWeapon::AssaultRifleSilver:	 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("AssaultRifleSilver"),	TEXT(""));	break;
		case EWeapon::AssaultRifleDark:		 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("AssaultRifleDark"),		TEXT(""));	break;

		// WeaponType == Pistol
		case EWeapon::PistolSilver:			 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("PistolSilver"),			TEXT(""));	break;
		case EWeapon::PistolDark:			 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("PistolDark"),			TEXT(""));	break;

		// WeaponType == Shotgun
		case EWeapon::ShotgunSilver:		 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("ShotgunSilver"),			TEXT(""));	break;
		case EWeapon::ShotgunDark:			 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("ShotgunDark"),			TEXT(""));	break;

		// WeaponType == Sniper
		case EWeapon::SniperSilver:			 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("SniperSilver"),			TEXT(""));	break;
		case EWeapon::SniperDark:			 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("SniperDark"),			TEXT(""));	break;

		// WeaponType == GrenadeLauncher
		case EWeapon::GrenadeLauncherSilver: WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("GrenadeLauncherSilver"),	TEXT(""));	break;
		case EWeapon::GrenadeLauncherDark:	 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("GrenadeLauncherDark"),	TEXT(""));	break;

		// WeaponType == RocketLauncher
		case EWeapon::RocketLauncherSilver:	 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("RocketLauncherSilver"),	TEXT(""));	break;
		case EWeapon::RocketLauncherDark:	 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("RocketLauncherDark"),	TEXT(""));	break;

		// WeaponType == SMG
		case EWeapon::SMG:					 WeaponRow = WeaponDataTable->FindRow<FWeaponDataTable>(FName("SMG"),					TEXT(""));	break;
		}

		if (WeaponRow)
		{
			WeaponMesh->SetSkeletalMesh(WeaponRow->Mesh);
			WeaponType = WeaponRow->WeaponType;
			AmmoType = WeaponRow->AmmoType;
			Ammo = WeaponRow->Ammo;
			AmmoMagazineCapacity = WeaponRow->AmmoMagazineCapacity;
			AmmoIconTexture = WeaponRow->AmmoIconTexture;
			WeaponIconTexture = WeaponRow->WeaponIconTexture;
			ReloadMontageSection = WeaponRow->ReloadMontageSection;
			SetItemName(WeaponRow->WeaponName);
			SetMaterialInstance(WeaponRow->MaterialInstance);
			SetMaterialIndex(WeaponRow->MaterialIndex);
		}

		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(FName("FresnelColor"), GlowColor);
			WeaponMesh->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
			EnableGlowMaterial();
		}
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// TODO : IN A TABLE DATA
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

void AWeapon::DisplayWidget()
{
	Super::DisplayWidget();

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// Choose Widget based on what Character wants to do : SWAP or PICK UP
	if (BaseCharacter)
	{
		if (BaseCharacter->GetWeaponInInventory(GetWeaponType()))	// SWAP WIDGET
		{
			GetPickupWidget()->SetDrawSize(FVector2D(360.0f, 300.0f));
			GetPickupWidget()->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

			if (WeaponSwitchWidgetClass)
			{
				GetPickupWidget()->SetWidgetClass(WeaponSwitchWidgetClass);

				USwitchWeaponWidget* SwitchWeaponWidget = Cast<USwitchWeaponWidget>(GetPickupWidget()->GetUserWidgetObject());
				if (SwitchWeaponWidget)
				{
					SwitchWeaponWidget->SetWeaponToTakeReference(this);
					SwitchWeaponWidget->SetWeaponInInventoryReference(BaseCharacter->GetWeaponInInventory(GetWeaponType()));
				}
			}
		}
		else	// PICK UP WIDGET
		{
			GetPickupWidget()->SetDrawSize(FVector2D(360.0f, 125.0f));
			GetPickupWidget()->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));

			if (WeaponPickupWidgetClass)
			{
				GetPickupWidget()->SetWidgetClass(WeaponPickupWidgetClass);

				UPickupWidget* PickupWidget = Cast<UPickupWidget>(GetPickupWidget()->GetUserWidgetObject());
				if (PickupWidget)
				{
					PickupWidget->SetWeaponReference(this);
				}
			}
		}
	}
}

void AWeapon::HideWidget()
{
	Super::HideWidget();

	// Delete last created widgets
	USwitchWeaponWidget* SwitchWeaponWidget = Cast<USwitchWeaponWidget>(GetPickupWidget()->GetUserWidgetObject());
	if (SwitchWeaponWidget)
	{
		SwitchWeaponWidget->DeleteCreatedWidgets();
	}
}

