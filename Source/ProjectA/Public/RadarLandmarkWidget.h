// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarLandmarkWidget.generated.h"

/** Forward Declarations */
class UImage;

UCLASS()
class PROJECTA_API URadarLandmarkWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	UTexture2D* LandmarkIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* IconImage;
};

