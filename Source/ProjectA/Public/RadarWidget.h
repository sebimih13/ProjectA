// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarWidget.generated.h"

/** Forward Declarations */
class UOverlay;
class URadarDirectionWidget;

UCLASS()
class PROJECTA_API URadarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	URadarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TSubclassOf<URadarDirectionWidget> RadarDirectionWidgetClass;

	/** Widget Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UOverlay* MainOverlay;

	/** Widget Animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* BlendOutAnimation;

private:
	TArray<URadarDirectionWidget*> DirectionWidgets;

public:
	void AddDirection(FString Name, int32 WorldRotation);

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE TArray<URadarDirectionWidget*> GetDirectionWidgets() const { return DirectionWidgets; };
};

