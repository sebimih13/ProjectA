// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/MarkerType.h"
#include "RadarMarkerWidget.generated.h"

/** Forward Declarations */
class UTextBlock;
class UImage;

UENUM(BlueprintType)
enum class EArrowDirection : uint8
{
	Up		UMETA(DisplayName = "Up"),
	Down	UMETA(DisplayName = "Down"),
	None	UMETA(DisplayName = "None")
};

UCLASS()
class PROJECTA_API URadarMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	EMarkerType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	UTexture2D* MainQuestIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	UTexture2D* SideQuestIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FLinearColor MainQuestColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	FLinearColor SideQuestColor;

	/** Widgets Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DistanceText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* MarkerImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ArrowImage;

private:
	int32 MaxDisplayedDistance = 999;

public:
	void UpdateDistance(int32 NewDistance);
	void SetArrowDirection(EArrowDirection Direction);
};

