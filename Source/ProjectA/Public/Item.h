// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

/** Forward Declarations */
class UBoxComponent;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	Pickup				UMETA(DisplayName = "Pickup"),
	EquipInterping		UMETA(DisplayName = "EquipInterping"),
	PickedUp			UMETA(DisplayName = "PickedUp"),
	Equipped			UMETA(DisplayName = "Equipped"),
	Falling				UMETA(DisplayName = "Falling")
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
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	/** Enables item tracing when overlapped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* PickupWidgetComponent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	FString ItemName = FString("Default");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 ItemsCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 DollarsCount = 1;

private:
	EItemState ItemState = EItemState::Pickup;

protected:
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void SetItemProperties();

public:
	/** FORCEINLINE Setters / Getters */
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; };
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; };
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidgetComponent; };

	FORCEINLINE EItemState GetItemState() const { return ItemState; };
	void SetItemState(EItemState DesiredState);
};

