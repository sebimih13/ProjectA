// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Headers/WeaponType.h"
#include "InventoryWheelWidget.generated.h"

/** Forward Declarations */
class UImage;
class UTextBlock;

class ABaseCharacterPlayerController;
class ABaseCharacter;

UCLASS()
class PROJECTA_API UInventoryWheelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	UInventoryWheelWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/** References */
	ABaseCharacter* BaseCharacter;
	ABaseCharacterPlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* BaseMaterialInstance;

	/** TODO : if we want to change the segments of the wheel
		MIN = 2
		MAX = 8
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	int32 Segments = 8;

	/** Dynamic Instance that can be changed at runtime */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float JoystickDeadzone = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess = "true"))
	float MouseCursorDeadzone = 15.0f;

	FVector2D CurrentInput = FVector2D::ZeroVector;
	int32 CurrentIndex = 0;

	/** Weapon Inventory
		0 : Unarmed
		1 : Shotgun
		2 : Pistol
		3 : Rocket Launcher
		4 : Sniper Rifle
		5 : Grenade Launcher
		6 : Assault Rifle
		7 : SMG
	*/
	TMap<int32, EWeaponType> IndexToWeaponType;
	TMap<int32, UImage*> IndexToIndexImage;
	TMap<int32, UTextBlock*> IndexToIndexText;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* WheelImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WeaponNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index1Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index2Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index3Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index4Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index5Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index6Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* Index7Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index1Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index2Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index3Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index4Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index5Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index6Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Index7Text;

private:
	float CalculateSegmentDegreeSize() const;

	void InitializeTMaps();

	void UpdateCurrentInput();
	void UpdateDirectionWithJoystick();
	void UpdateDirectionWithMouse();

	void UpdateIndex();
	void UpdateDynamicMaterialParameters();
	
	/** FORCEINLINE Setters / Getters */
	FORCEINLINE EWeaponType GetWeaponTypeOnIndex(int32 Index) const { return IndexToWeaponType[Index]; };
	FORCEINLINE UImage* GetImageOnIndex(int32 Index) const { return IndexToIndexImage[Index]; };
	FORCEINLINE UTextBlock* GetTextOnIndex(int32 Index) const { return IndexToIndexText[Index]; };
};

