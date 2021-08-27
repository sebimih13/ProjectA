// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupWidget.generated.h"

/** Forward Declarations */
class UTextBlock;
class UImage;
class AWeapon;
class ABaseCharacterPlayerController;

UCLASS()
class PROJECTA_API UPickupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	UPickupWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/** References */
	AWeapon* Weapon;
	ABaseCharacterPlayerController* PlayerController;

public:
	/** DualShock 4 Controller Button Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* DS4Icon;

	/** Xbox Controller Button Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* XBIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* AmmoImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DollarIcon1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DollarIcon2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DollarIcon3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DollarIcon4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DollarIcon5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ControllerButtonIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* IconKey;

public:
	/** FORCEINLINE Setters / Getters */
	void SetWeaponReference(AWeapon* WeaponReference) { Weapon = WeaponReference; };
};

