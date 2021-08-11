// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseCharacterHUD.generated.h"

/** Forward Declarations */
class ABaseCharacter;

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

private:
	/** References */
	ABaseCharacter* Character;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	UTexture* CrosshairTop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	UTexture* CrosshairBottom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	UTexture* CrosshairRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	UTexture* CrosshairLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	float CrosshairSpreadMax = 16.0f;

	/** Determines the spread of the crosshair */
	float CrosshairSpreadMultiplier = 0.0f;

	/** Components for crosshair spread */
	float CrosshairVelocityFactor = 0.0f;
	float CrosshairInAirFactor = 0.0f;
	float CrosshairAimFactor = 0.0f;
	float CrosshairShootingFactor = 0.0f;

private:
	FVector2D CrosshairCenter;

private:
	/** Calculate functions */
	void CalculateCrosshairSpread(float DeltaTime);
};

