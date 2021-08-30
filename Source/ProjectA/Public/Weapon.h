// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Headers/AmmoType.h"
#include "Headers/WeaponType.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

/** Forward Declarations */
class USwitchWeaponWidget;
class UPickupWidget;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common				UMETA(DisplayName = "Common"),
	Uncommon			UMETA(DisplayName = "Uncommon"),
	Rare				UMETA(DisplayName = "Rare"),
	Epic				UMETA(DisplayName = "Epic"),
	Legendary			UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EWeapon : uint8
{
	AssaultRifleSilver			UMETA(DisplayName = "AssaultRifleSilver"),
	AssaultRifleDark			UMETA(DisplayName = "AssaultRifleDark"),

	PistolSilver				UMETA(DisplayName = "PistolSilver"),
	PistolDark					UMETA(DisplayName = "PistolDark"),

	ShotgunSilver				UMETA(DisplayName = "ShotgunSilver"),
	ShotgunDark					UMETA(DisplayName = "ShotgunDark"),

	SniperSilver				UMETA(DisplayName = "SniperSilver"),
	SniperDark					UMETA(DisplayName = "SniperDark"),

	GrenadeLauncherSilver		UMETA(DisplayName = "GrenadeLauncherSilver"),
	GrenadeLauncherDark			UMETA(DisplayName = "GrenadeLauncherDark"),

	RocketLauncherSilver		UMETA(DisplayName = "RocketLauncherSilver"),
	RocketLauncherDark			UMETA(DisplayName = "RocketLauncherDark"),

	SMG							UMETA(DisplayName = "SMG"),
};

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AmmoMagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* AmmoIconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* WeaponIconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaterialIndex = 0;
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor LightColorBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor DarkColorBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CustomDepthStencil;
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

	virtual void SetItemProperties() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPickupWidget> WeaponPickupWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USwitchWeaponWidget> WeaponSwitchWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeapon Weapon = EWeapon::AssaultRifleSilver;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 Ammo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 AmmoMagazineCapacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UTexture2D* WeaponIconTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection;

	/** Weapen Rarity */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemRarityDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity = EItemRarity::Common;

	/** Color in the glow material */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FLinearColor GlowColor = FLinearColor::Black;

	/** Light Color Background in the Pickup Widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FLinearColor LightColorBackground = FLinearColor::Black;;

	/** Dark Color Background in the Pickup Widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FLinearColor DarkColorBackground = FLinearColor::Black;

	/** Number of Stars in the Pickup Widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfStars = 0;

	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime = 0.7f;
	bool bFalling = false;

private:
	void StopFalling();

public:
	/** Adds an impulse to the Weapon */
	void ThrowWeapon();

	void DecrementAmmo();

	void AddAmmo(int32 Amount);

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;

	virtual void DisplayWidget() override;
	virtual void HideWidget() override;

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

	FORCEINLINE FLinearColor GetLightColorBackground() const { return LightColorBackground; };
	FORCEINLINE FLinearColor GetDarkColorBackground() const { return DarkColorBackground; };
	FORCEINLINE int32 GetNumberOfStars() const { return NumberOfStars; };
};

