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

	/** Top Background */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* RightTopBackground;

	/** Bottom Background */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* RightBottomBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmountText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* AmmoImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StarIcon1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StarIcon2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StarIcon3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StarIcon4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* StarIcon5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
	UImage* ControllerButtonIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
	UTextBlock* IconKey;

public:
	/** FORCEINLINE Setters / Getters */
	void SetWeaponReference(AWeapon* WeaponReference) { Weapon = WeaponReference; };
	AWeapon* GetWeaponReference() const { return Weapon; };	// todo
};

