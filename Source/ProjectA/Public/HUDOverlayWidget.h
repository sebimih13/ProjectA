// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDOverlayWidget.generated.h"

/** Forward Declarations */
class UCanvasPanel;

class UAmmoWidget;

UCLASS()
class PROJECTA_API UHUDOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Constructor */
	UHUDOverlayWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UAmmoWidget* AmmoWidget;

public:
	void SetAmmoWidgetVisibility(bool bVisible);
};

