// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoPickupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Ammo.h"

UAmmoPickupWidget::UAmmoPickupWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void UAmmoPickupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Ammo)
	{
		AmmoCountText->SetText(FText::FromString(FString::FromInt(Ammo->GetItemsCount())));
		AmmoImage->SetBrushFromTexture(Ammo->GetAmmoIconTexture());
	}
}

