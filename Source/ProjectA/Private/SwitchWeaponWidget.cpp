// Fill out your copyright notice in the Description page of Project Settings.

#include "SwitchWeaponWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBoxSlot.h"

#include "PickupWidget.h"
#include "Weapon.h"
#include "BaseCharacterPlayerController.h"

USwitchWeaponWidget::USwitchWeaponWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void USwitchWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set Reference
	PlayerController = Cast<ABaseCharacterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	// Create the Weapon Pickup Widgets
	CreateWeaponPickupWidget(WeaponInInventoryPickupWidget, WeaponInInventory, WeaponInInventoryBox);
	CreateWeaponPickupWidget(WeaponToTakePickupWidget, WeaponToTake, WeaponToTakeBox);
}

void USwitchWeaponWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

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

	if (WeaponInInventoryPickupWidget && WeaponInInventoryPickupWidget->GetWeaponReference() != WeaponInInventory)
	{
		// Remove from parent and viewport
		WeaponInInventoryPickupWidget->RemoveFromParent();

		// Create the new Weapon Pickup Widget
		CreateWeaponPickupWidget(WeaponInInventoryPickupWidget, WeaponInInventory, WeaponInInventoryBox);
	}
}

void USwitchWeaponWidget::DeleteCreatedWidgets()
{
	WeaponInInventoryPickupWidget->RemoveFromParent();
	WeaponToTakePickupWidget->RemoveFromParent();
}

void USwitchWeaponWidget::CreateWeaponPickupWidget(UPickupWidget*& WidgetToHold, AWeapon* Weapon, UVerticalBox* Box)
{
	WidgetToHold = CreateWidget<UPickupWidget>(this, WeaponPickupWidgetClass);
	if (WidgetToHold && Weapon)
	{
		WidgetToHold->SetWeaponReference(Weapon);
		Box->AddChildToVerticalBox(WidgetToHold);
		UVerticalBoxSlot* VerticalBoxSlot = UWidgetLayoutLibrary::SlotAsVerticalBoxSlot(WidgetToHold);

		if (VerticalBoxSlot)
		{
			VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
	}
}

