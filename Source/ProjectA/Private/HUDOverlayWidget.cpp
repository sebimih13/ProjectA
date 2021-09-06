// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDOverlayWidget.h"

#include "Blueprint/WidgetTree.h"

#include "AmmoWidget.h"

UHUDOverlayWidget::UHUDOverlayWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void UHUDOverlayWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);

	for (UWidget* Widget : Widgets)
	{
		// TODO : DEBUG
		FName WidgetName = Widget->GetFName();
		UE_LOG(LogTemp, Warning, TEXT("WIDGET FOUND : %s"), *WidgetName.ToString());

		if (!AmmoWidget)
		{
			AmmoWidget = Cast<UAmmoWidget>(Widget);
		}
	}

	if (AmmoWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMMO WIDGET FOUND"));
	}
}

void UHUDOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHUDOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UHUDOverlayWidget::SetAmmoWidgetVisibility(bool bVisible)
{
	if (AmmoWidget)
	{
		AmmoWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

