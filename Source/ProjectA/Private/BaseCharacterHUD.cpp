// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterHUD.h"

ABaseCharacterHUD::ABaseCharacterHUD()
{

}

void ABaseCharacterHUD::BeginPlay()
{
	Super::BeginPlay();


}

void ABaseCharacterHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ABaseCharacterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	DrawTexture(CrosshairTexture, static_cast<float>(ViewportSize.X / 2) - 32.0f, static_cast<float>(ViewportSize.Y / 2) - 32.0f, 64.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f);
}

