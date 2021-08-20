// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Headers/AmmoType.h"
#include "Ammo.generated.h"

/** Forward Declarations */


UCLASS()
class PROJECTA_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	AAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetItemProperties() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void CollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	/** Mesh for the Ammo Pickup */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType = EAmmoType::AssaultRifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture = nullptr;

public:
	/** FORCEINLINE Setters / Getters */
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; };

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };
	FORCEINLINE UTexture2D* GetAmmoIconTexture() const { return AmmoIconTexture; };
};

