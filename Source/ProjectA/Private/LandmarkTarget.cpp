// Fill out your copyright notice in the Description page of Project Settings.

#include "LandmarkTarget.h"
#include "Components/BillboardComponent.h"

// Sets default values
ALandmarkTarget::ALandmarkTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SetRootComponent(Billboard);
	Billboard->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
}

void ALandmarkTarget::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Billboard->SetSprite(IconOnRadar);
}

// Called when the game starts or when spawned
void ALandmarkTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALandmarkTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

