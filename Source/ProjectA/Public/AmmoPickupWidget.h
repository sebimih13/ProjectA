// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoPickupWidget.generated.h"

/** Forward Declarations */
class UTextBlock;
class UImage;
class AAmmo;

UCLASS()
class PROJECTA_API UAmmoPickupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	UAmmoPickupWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

private:
	/** References */
	AAmmo* Ammo;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmmoCountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* AmmoImage;

public:
	/** FORCEINLINE Setters / Getters */
	void SetAmmoReference(AAmmo* AmmoReference) { Ammo = AmmoReference; };
};

