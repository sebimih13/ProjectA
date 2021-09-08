// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarLandmarkWidget.h"
#include "Components/Image.h"

void URadarLandmarkWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!LandmarkIcon) return;

	IconImage->SetBrushFromTexture(LandmarkIcon);
	IconImage->SetBrushSize(FVector2D(LandmarkIcon->GetSizeX(), LandmarkIcon->GetSizeY()));
}

