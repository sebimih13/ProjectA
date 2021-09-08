// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarMarkerWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void URadarMarkerWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	switch (Type)
	{
	case EMarkerType::MainQuest: MarkerImage->SetBrushFromTexture(MainQuestIcon, true); 
								 ArrowImage->SetColorAndOpacity(MainQuestColor);
		break;

	case EMarkerType::SideQuest: MarkerImage->SetBrushFromTexture(SideQuestIcon, true);
								 ArrowImage->SetColorAndOpacity(SideQuestColor);
		break;
	}
}

void URadarMarkerWidget::UpdateDistance(int32 NewDistance)
{
	FString DistanceString;
	if (NewDistance > MaxDisplayedDistance)
	{
		DistanceString = FString::FromInt(MaxDisplayedDistance) + "+";
	}
	else
	{
		DistanceString = FString::FromInt(FMath::Clamp(NewDistance, 0, MaxDisplayedDistance)) + "m";
	}

	DistanceText->SetText(FText::FromString(DistanceString));
}

void URadarMarkerWidget::SetArrowDirection(EArrowDirection Direction)
{
	switch (Direction)
	{
	case EArrowDirection::Up:	ArrowImage->SetRenderScale(FVector2D(1.0f, 1.0f));	break;
	case EArrowDirection::Down: ArrowImage->SetRenderScale(FVector2D(1.0f, -1.0f)); break;
	case EArrowDirection::None: ArrowImage->SetRenderScale(FVector2D(0.0f, 0.0f));	break;
	}
}

