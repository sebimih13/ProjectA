// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupWidget.h"
#include "Item.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UPickupWidget::UPickupWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	
}

void UPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Item)
	{
		ItemNameText->SetText(FText::FromString(Item->ItemName));
		AmountText->SetText(FText::FromString(FString::FromInt(Item->ItemsCount)));

		if (Item->DollarsCount >= 1)
		{
			DollarIcon1->SetOpacity(1.0f);
		}
		if (Item->DollarsCount >= 2)
		{
			DollarIcon2->SetOpacity(1.0f);
		}
		if (Item->DollarsCount >= 3)
		{
			DollarIcon3->SetOpacity(1.0f);
		}
		if (Item->DollarsCount >= 4)
		{
			DollarIcon4->SetOpacity(1.0f);
		}
		if (Item->DollarsCount >= 5)
		{
			DollarIcon5->SetOpacity(1.0f);
		}
	}
}

