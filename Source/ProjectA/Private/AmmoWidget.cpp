// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "BaseCharacter.h"
#include "Weapon.h"

UAmmoWidget::UAmmoWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void UAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Character = Cast<ABaseCharacter>(GetOwningPlayerPawn());
}

void UAmmoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Character && Character->GetWeapon())
	{
		CurrentAmmoText->SetText(FText::FromString(FString::FromInt(Character->GetWeapon()->GetAmmo())));
		WeaponNameText->SetText(FText::FromString(Character->GetWeapon()->ItemName));
	}
	else
	{
		CurrentAmmoText->SetText(FText::FromString("NO"));
		WeaponNameText->SetText(FText::FromString("NO WEAPON"));
	}
}

