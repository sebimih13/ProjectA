// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoPickupWidget.generated.h"

/** Forward Declarations */
class UTextBlock;
class UImage;
class AAmmo;
class ABaseCharacterPlayerController;

UCLASS()
class PROJECTA_API UAmmoPickupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	UAmmoPickupWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/** References */
	AAmmo* Ammo;
	ABaseCharacterPlayerController* PlayerController;

public:
	/** DualShock 4 Controller Button Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* DS4Icon;

	/** Xbox Controller Button Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* XBIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoCountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* AmmoImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ControllerButtonIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ButtonIconText;

public:
	/** FORCEINLINE Setters / Getters */
	void SetAmmoReference(AAmmo* AmmoReference) { Ammo = AmmoReference; };
};

