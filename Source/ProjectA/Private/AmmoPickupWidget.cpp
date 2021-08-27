// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoPickupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Ammo.h"
#include "BaseCharacterPlayerController.h"

UAmmoPickupWidget::UAmmoPickupWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void UAmmoPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set Reference
	PlayerController = Cast<ABaseCharacterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Ammo)
	{
		AmmoCountText->SetText(FText::FromString(FString::FromInt(Ammo->GetItemsCount())));
		AmmoImage->SetBrushFromTexture(Ammo->GetAmmoIconTexture());
	}
}

void UAmmoPickupWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Update Button
	if (PlayerController)
	{
		if (PlayerController->GetInputType() == EInputType::KeyboardMouse)
		{
			ButtonIconText->SetVisibility(ESlateVisibility::Visible);
			ControllerButtonIcon->SetOpacity(0.0f);
		}
		else
		{
			switch (PlayerController->GetControllerType())
			{
			case EControllerType::DualShock4:	ControllerButtonIcon->SetBrushFromTexture(DS4Icon);	break;
			case EControllerType::Xbox:			ControllerButtonIcon->SetBrushFromTexture(XBIcon);	break;
			}

			ButtonIconText->SetVisibility(ESlateVisibility::Hidden);
			ControllerButtonIcon->SetOpacity(1.0f);
		}
	}
}

