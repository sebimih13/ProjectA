// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "BaseCharacterPlayerController.h"

UPickupWidget::UPickupWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	
}

void UPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set Reference
	PlayerController = Cast<ABaseCharacterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (Weapon)
	{
		ItemNameText->SetText(FText::FromString(Weapon->GetItemName()));
		AmountText->SetText(FText::FromString(FString::FromInt(Weapon->GetItemsCount())));
		AmmoImage->SetBrushFromTexture(Weapon->GetAmmoIconTexture());

		if (Weapon->GetDollarsCount() >= 1)
		{
			DollarIcon1->SetOpacity(1.0f);
		}
		if (Weapon->GetDollarsCount() >= 2)
		{
			DollarIcon2->SetOpacity(1.0f);
		}
		if (Weapon->GetDollarsCount() >= 3)
		{
			DollarIcon3->SetOpacity(1.0f);
		}
		if (Weapon->GetDollarsCount() >= 4)
		{
			DollarIcon4->SetOpacity(1.0f);
		}
		if (Weapon->GetDollarsCount() >= 5)
		{
			DollarIcon5->SetOpacity(1.0f);
		}
	}
}

void UPickupWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ControllerButtonIcon || !IconKey) return;

	// Update Button
	if (PlayerController)
	{
		if (PlayerController->GetInputType() == EInputType::KeyboardMouse)
		{
			IconKey->SetVisibility(ESlateVisibility::Visible);
			ControllerButtonIcon->SetOpacity(0.0f);
		}
		else
		{
			switch (PlayerController->GetControllerType())
			{
			case EControllerType::DualShock4:	ControllerButtonIcon->SetBrushFromTexture(DS4Icon);	break;
			case EControllerType::Xbox:			ControllerButtonIcon->SetBrushFromTexture(XBIcon);	break;
			}

			IconKey->SetVisibility(ESlateVisibility::Hidden);
			ControllerButtonIcon->SetOpacity(1.0f);
		}
	}
}

