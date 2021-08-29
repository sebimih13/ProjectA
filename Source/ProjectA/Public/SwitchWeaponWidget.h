// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SwitchWeaponWidget.generated.h"

/** Forward Declarations */
class UVerticalBox;
class UImage;
class UTextBlock;

class UPickupWidget;
class AWeapon;
class ABaseCharacterPlayerController;


UCLASS()
class PROJECTA_API USwitchWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	USwitchWeaponWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/** References */
	AWeapon* WeaponInInventory;
	AWeapon* WeaponToTake;

	UPickupWidget* WeaponInInventoryPickupWidget;
	UPickupWidget* WeaponToTakePickupWidget;

	ABaseCharacterPlayerController* PlayerController;

public:
	/** DualShock 4 Controller Button Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* DS4Icon;

	/** Xbox Controller Button Image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D* XBIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<UPickupWidget> WeaponPickupWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* WeaponInInventoryBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* WeaponToTakeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ControllerButtonIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* IconKey;

private:
	void CreateWeaponPickupWidget(UPickupWidget*& WidgetToHold, AWeapon* Weapon, UVerticalBox* Box);

public:
	void DeleteCreatedWidgets();

	/** FORCEINLINE Setters / Getters */
	void SetWeaponInInventoryReference(AWeapon* WeaponReference) { WeaponInInventory = WeaponReference; };
	void SetWeaponToTakeReference(AWeapon* WeaponReference) { WeaponToTake = WeaponReference; };
};

