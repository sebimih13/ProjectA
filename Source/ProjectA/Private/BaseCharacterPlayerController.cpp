// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWheelWidget.h"

ABaseCharacterPlayerController::ABaseCharacterPlayerController()
{

}

void ABaseCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
	}

	if (HUDOverlay)
	{
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABaseCharacterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("LookRight/Left", this, &ABaseCharacterPlayerController::UpdateGamepadRightThumbstickX).bConsumeInput = false;
	InputComponent->BindAxis("LookUp/Down", this, &ABaseCharacterPlayerController::UpdateGamepadRightThumbstickY).bConsumeInput = false;
}

// Input Functions
void ABaseCharacterPlayerController::UpdateGamepadRightThumbstickX(float Value)
{
	GamepadInput.X = Value;
}

void ABaseCharacterPlayerController::UpdateGamepadRightThumbstickY(float Value)
{
	GamepadInput.Y = Value;
}

void ABaseCharacterPlayerController::DisplayInventoryWheel()
{
	if (InventoryWheelClass)
	{
		InventoryWheelWidget = CreateWidget<UInventoryWheelWidget>(this, InventoryWheelClass);
	}

	if (!InventoryWheelWidget) return;

	InventoryWheelWidget->AddToViewport();

	FInputModeGameAndUI InputModeGameAndUI;
	SetInputMode(InputModeGameAndUI);
	bShowMouseCursor = true;

	// Set Mouse Position in the middle of viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	int CenterMouseX = FMath::FloorToInt(ViewportSize.X / 2.0f);
	int CenterMouseY = FMath::FloorToInt(ViewportSize.Y / 2.0f);
	SetMouseLocation(CenterMouseX, CenterMouseY);
}

void ABaseCharacterPlayerController::RemoveInventoryWheel()
{
	if (!InventoryWheelWidget) return;

	InventoryWheelWidget->RemoveFromViewport();

	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
	bShowMouseCursor = false;
}

