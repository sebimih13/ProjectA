// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarWidget.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "RadarDirectionWidget.h"
#include "RadarMarkerWidget.h"
#include "RadarLandmarkWidget.h"

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

void URadarWidget::AddMarker(EMarkerType Type)
{
	if (!RadarMarkerWidgetClass) return;

	URadarMarkerWidget* MarkerWidget = CreateWidget<URadarMarkerWidget>(this, RadarMarkerWidgetClass);
	MarkerWidget->Type = Type;
	MarkerWidget->SetVisibility(ESlateVisibility::Hidden);

	MarkerWidgets.Add(MarkerWidget);

	UOverlaySlot* OverlaySlot = MainOverlay->AddChildToOverlay(MarkerWidget);
	OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	OverlaySlot->SetPadding(FMargin(0.0f, 5.0f, 0.0f, 0.0f));
}

void URadarWidget::AddLandmark(UTexture2D* Icon)
{
	if (!LandmarkWidgetClass) return;

	URadarLandmarkWidget* LandmarkWidget = CreateWidget<URadarLandmarkWidget>(this, LandmarkWidgetClass);
	LandmarkWidget->LandmarkIcon = Icon;
	LandmarkWidget->SetVisibility(ESlateVisibility::Hidden);

	LandmarkWidgets.Add(LandmarkWidget);

	UOverlaySlot* OverlaySlot = MainOverlay->AddChildToOverlay(LandmarkWidget);
	OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
}

