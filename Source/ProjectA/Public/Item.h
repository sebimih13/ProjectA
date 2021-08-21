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
class UCurveVector;
class UCurveFloat;
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

	virtual void OnConstruction(const FTransform& Transform) override;

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

	/** Curve to drive the dynamic material parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	UCurveVector* CurveInterpPulse = nullptr;

	/** Duration of the curve and timer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	float ZCurveTime = 0.7f;

	/** Curve to drive the dynamic material parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	UCurveVector* CurvePulse = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	float PulseCurveTime = 5.0f;

	/** Sound played when Item is picked up */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	USoundCue* PickupSound;

	/** Sound played when Item is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	USoundCue* EquipSound;

	/** Index for the material we change at runtime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties")
	int32 MaterialIndex = 0;

	/** Material Instance used with the Dynamic Material Instance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	UMaterialInstance* MaterialInstance;

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

	/** Dynamic Instance that can be changed at runtime */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	bool bCanChangeCustomDepth = true;

	/** Change Dynamic Material Parameters */
	FTimerHandle PulseTimer;
	float GlowAmount = 150.0f;
	float FresnelExponent = 3.0f;
	float FresnelReflectFraction = 4.0f;

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

	/** Sound */
	void PlayPickupSound();
	void PlayEquipSound();

	void InitializeCustomDepth();

	/** Pulse System */
	void ResetPulseTimer();
	void UpdatePulse();
		 
public:
	void StartItemCurve(ABaseCharacter* BaseCharacter);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();

	void EnableGlowMaterial();
	void DisableGlowMaterial();

	/** Start Pulse */
	void StartPulseTimer();

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; };
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidgetComponent; };

	FORCEINLINE EItemState GetItemState() const { return ItemState; };
	void SetItemState(EItemState DesiredState);

	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() const { return DynamicMaterialInstance; };

	FORCEINLINE bool GetCanChangeCustomDepth() const { return bCanChangeCustomDepth; };
};

