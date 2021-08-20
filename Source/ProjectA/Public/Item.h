// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

/** Forward Declarations */
class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class USoundCue;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	Pickup				UMETA(DisplayName = "Pickup"),
	EquipInterping		UMETA(DisplayName = "EquipInterping"),
	PickedUp			UMETA(DisplayName = "PickedUp"),
	Equipped			UMETA(DisplayName = "Equipped"),
	Falling				UMETA(DisplayName = "Falling")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Ammo				UMETA(DisplayName = "Ammo"),
	Weapon				UMETA(DisplayName = "Weapon")
};

UCLASS()
class PROJECTA_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	/** Enables item tracing when overlapped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidgetComponent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemType ItemType = EItemType::Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	FString ItemName = FString("Default");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 ItemsCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 DollarsCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	UCurveFloat* CurveItemZ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	UCurveFloat* CurveItemScale = nullptr;

	/** Duration of the curve and timer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	float ZCurveTime = 0.7f;

	/** Sound played when Item is picked up */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	USoundCue* PickupSound;

	/** Sound played when Item is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	USoundCue* EquipSound;

private:
	/** References */
	ABaseCharacter* Character;

	EItemState ItemState = EItemState::Pickup;

	/** Plays when we start interping */
	FTimerHandle ItemInterpTimer;
	bool bInterping = false;

	/** Initial Yaw Offset between the camera and the interping item */
	float InterpInitialYawOffset = 0.0f;

	/** Index of the InterpLocations[] this item is interping to */
	int32 InterpLocationIndex = 0;

protected:
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SetItemProperties();

private:
	/** Called when ItemInterpTimer is finished */
	void FinishInterping();

	/** Handles item interpolation in the EquipInterping state */
	void ItemInterping(float DeltaTime);

	/** Get Interp Location based on the item type */
	FVector GetInterpLocation();

	void PlayPickupSound();
	void PlayEquipSound();
		 
public:
	void StartItemCurve(ABaseCharacter* BaseCharacter);

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; };
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidgetComponent; };

	FORCEINLINE EItemState GetItemState() const { return ItemState; };
	void SetItemState(EItemState DesiredState);
};

