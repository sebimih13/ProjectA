// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterHUD.h"
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacterHUD::ABaseCharacterHUD()
{

}

void ABaseCharacterHUD::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ABaseCharacter>(GetOwningPawn());
}

void ABaseCharacterHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Calculate CrosshairCenter
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	CrosshairCenter.X = ViewportSize.X / 2 - 32.0f;
	CrosshairCenter.Y = ViewportSize.Y / 2 - 32.0f;

	CalculateCrosshairSpread(DeltaTime);
}

void ABaseCharacterHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawTexture(CrosshairBottom, CrosshairCenter.X, CrosshairCenter.Y + CrosshairSpreadMultiplier * CrosshairSpreadMax, 64.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	DrawTexture(CrosshairTop, CrosshairCenter.X, CrosshairCenter.Y - CrosshairSpreadMultiplier * CrosshairSpreadMax, 64.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	DrawTexture(CrosshairRight, CrosshairCenter.X + CrosshairSpreadMultiplier * CrosshairSpreadMax, CrosshairCenter.Y, 64.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	DrawTexture(CrosshairLeft, CrosshairCenter.X - CrosshairSpreadMultiplier * CrosshairSpreadMax, CrosshairCenter.Y, 64.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f);
}

void ABaseCharacterHUD::CalculateCrosshairSpread(float DeltaTime)
{
	// Calculate CrosshairVelocityFactor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, Character->RunSpeed), FVector2D(0.0f, 1.0f), Character->GetSpeed());

	// Calculate CrosshairInAirFactor
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.0f, DeltaTime, 30.0f);
	}

	// Calculate CrosshairAimFactor
	if (Character->GetIsAiming())
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.0f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.0f);
	}

	// Calculate CrosshairShootingFactor
	if (Character->GetIsFiringBullet())
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.0f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.0f, DeltaTime, 60.0f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

