// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterPlayerController.h"
#include "Blueprint/UserWidget.h"

ABaseCharacterPlayerController::ABaseCharacterPlayerController()
{

}

void ABaseCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDUOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
	}

	if (HUDUOverlay)
	{
		HUDUOverlay->AddToViewport();
		HUDUOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABaseCharacterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

