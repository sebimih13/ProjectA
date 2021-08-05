// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseCharacterHUD.generated.h"

/** Forward Declarations */


UCLASS()
class PROJECTA_API ABaseCharacterHUD : public AHUD
{
	GENERATED_BODY()

public:
	// Sets default values for this class
	ABaseCharacterHUD();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void DrawHUD() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	UTexture* CrosshairTexture;
};

