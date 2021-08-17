// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseCharacterPlayerController.generated.h"

/** Forward Declarations */
class UUserWidget;

UCLASS()
class PROJECTA_API ABaseCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Constructor : Sets default values for this character's properties */
	ABaseCharacterPlayerController();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<UUserWidget> HUDOverlayClass;

private:
	UUserWidget* HUDUOverlay;
};

