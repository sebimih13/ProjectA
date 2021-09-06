// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarDirectionWidget.h"
#include "Components/TextBlock.h"

void URadarDirectionWidget::NativePreConstruct()
{
	DirectionText->SetText(FText::FromString(Name.LeftChop(Name.Len() - 2).ToUpper()));
}

