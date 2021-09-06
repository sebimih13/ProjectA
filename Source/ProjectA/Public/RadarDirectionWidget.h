// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarDirectionWidget.generated.h"

/** Forward Declarations */
class UTextBlock;

UCLASS()
class PROJECTA_API URadarDirectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DirectionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	int32 WorldRotation;
};

