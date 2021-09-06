// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseCharacterPlayerController.generated.h"

/** Forward Declarations */
class UUserWidget;
class UHUDOverlayWidget;
class UInventoryWheelWidget;

UENUM(BlueprintType)
enum class EInputType : uint8
{
	Controller		UMETA(DisplayName = "Controller"),
	KeyboardMouse	UMETA(DisplayName = "KeyboardMouse")
};

UENUM(BlueprintType)
enum class EControllerType : uint8
{
	DualShock4		UMETA(DisplayName = "DualShock4"),
	Xbox			UMETA(DisplayName = "Xbox")
};

UCLASS()
class PROJECTA_API ABaseCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Constructor : Sets default values for this character's properties */
	ABaseCharacterPlayerController();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

public:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called to update Joystick Right Thumbstick Values */
	void UpdateGamepadRightThumbstickX(float Value);
	void UpdateGamepadRightThumbstickY(float Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<UUserWidget> HUDOverlayClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<UInventoryWheelWidget> InventoryWheelClass;

private:
	/** Input Type */
	EInputType InputType = EInputType::KeyboardMouse;

	/** Controller Type */
	EControllerType ControllerType = EControllerType::DualShock4;

	/** Gamepad Right Joystick Input */
	FVector2D GamepadInput = FVector2D::ZeroVector;

	/** Widget to hold the overlay created from HUDOverlayClass */
	UHUDOverlayWidget* HUDOverlay;

	/** Widget to hold the Inventory Wheel Widget created from InventoryWheelClass */
	UInventoryWheelWidget* InventoryWheelWidget;

public:
	void SetHUDOverlayVisibility(bool bVisible);
	void SetAmmoWidgetVisibility(bool bVisible);

	void DisplayInventoryWheel();
	void RemoveInventoryWheel();

	/** FORCEINLINE Setters / Getters */
	FORCEINLINE EInputType GetInputType() const { return InputType; };
	FORCEINLINE void SetInputType(EInputType Type) { InputType = Type; };

	FORCEINLINE EControllerType GetControllerType() const { return ControllerType; };
	FORCEINLINE void SetControllerType(EControllerType Type) { ControllerType = Type; };

	FORCEINLINE FVector2D GetGamepadInput() const { return GamepadInput; };

	FORCEINLINE UHUDOverlayWidget* GetHUDOverlayWidget() const { return HUDOverlay; };
};

