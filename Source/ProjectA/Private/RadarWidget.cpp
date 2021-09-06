// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarWidget.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "RadarDirectionWidget.h"

URadarWidget::URadarWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void URadarWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void URadarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void URadarWidget::AddDirection(FString Name, int32 WorldRotation)
{
	if (!RadarDirectionWidgetClass) return;

	URadarDirectionWidget* DirectionWidget = CreateWidget<URadarDirectionWidget>(this, RadarDirectionWidgetClass);
	DirectionWidget->Name = Name;
	DirectionWidget->WorldRotation = WorldRotation;
	DirectionWidget->SetVisibility(ESlateVisibility::Hidden);

	DirectionWidgets.Add(DirectionWidget);
	
	UOverlaySlot* OverlaySlot = MainOverlay->AddChildToOverlay(DirectionWidget); 
	OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
}

