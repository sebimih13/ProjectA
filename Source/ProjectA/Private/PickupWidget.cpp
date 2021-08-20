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
		ItemNameText->SetText(FText::FromString(Weapon->ItemName));
		AmountText->SetText(FText::FromString(FString::FromInt(Weapon->ItemsCount)));
		AmmoImage->SetBrushFromTexture(Weapon->GetAmmoIconTexture());

		if (Weapon->DollarsCount >= 1)
		{
			DollarIcon1->SetOpacity(1.0f);
		}
		if (Weapon->DollarsCount >= 2)
		{
			DollarIcon2->SetOpacity(1.0f);
		}
		if (Weapon->DollarsCount >= 3)
		{
			DollarIcon3->SetOpacity(1.0f);
		}
		if (Weapon->DollarsCount >= 4)
		{
			DollarIcon4->SetOpacity(1.0f);
		}
		if (Weapon->DollarsCount >= 5)
		{
			DollarIcon5->SetOpacity(1.0f);
		}
	}
}

