// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/MarkerType.h"
#include "RadarWidget.generated.h"

/** Forward Declarations */
class UOverlay;
class URadarDirectionWidget;
class URadarMarkerWidget;
class URadarLandmarkWidget;

UCLASS()
class PROJECTA_API URadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSubclassOf<URadarDirectionWidget> RadarDirectionWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSubclassOf<URadarMarkerWidget> RadarMarkerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSubclassOf<URadarLandmarkWidget> LandmarkWidgetClass;

	/** Widget Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UOverlay* MainOverlay;

	/** Widget Animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* BlendOutAnimation;

private:
	TArray<URadarDirectionWidget*> DirectionWidgets;
	TArray<URadarMarkerWidget*> MarkerWidgets;
	TArray<URadarLandmarkWidget*> LandmarkWidgets;

public:
	void AddDirection(FString Name, int32 WorldRotation);
	void AddMarker(EMarkerType Type);
	void AddLandmark(UTexture2D* Icon);

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE TArray<URadarDirectionWidget*> GetDirectionWidgets() const { return DirectionWidgets; };
	FORCEINLINE TArray<URadarMarkerWidget*> GetMarkerWidgets() const { return MarkerWidgets; };
	FORCEINLINE TArray<URadarLandmarkWidget*> GetLandmarkWidgets() const { return LandmarkWidgets; };
};

