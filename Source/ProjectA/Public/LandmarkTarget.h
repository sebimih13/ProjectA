// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LandmarkTarget.generated.h"

/** Forward Declarations */
class UBillboardComponent;

UCLASS()
class PROJECTA_API ALandmarkTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALandmarkTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billboard", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* Billboard;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Billboard")
	UTexture2D* IconOnRadar;
};

