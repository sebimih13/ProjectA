// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Weapon.h"

UPickupWidget::UPickupWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	
}

void UPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

