// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWheelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathVectorConstants.h"

#include "BaseCharacterPlayerController.h"
#include "BaseCharacter.h"
#include "Weapon.h"

UInventoryWheelWidget::UInventoryWheelWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void UInventoryWheelWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (BaseMaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterialInstance, this);

		if (DynamicMaterialInstance)
		{
			// Set Wheel Image from the Material
			WheelImage->SetBrushFromMaterial(DynamicMaterialInstance);

			// Fix Rotation
			WheelImage->SetRenderTransformAngle(CalculateSegmentDegreeSize() / 2 * -1.0f);
		}
	}
}

void UInventoryWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeTMap();

	if (BlendInAnimation)
	{
		PlayAnimation(BlendInAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
	}

	BaseCharacter = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	if (BaseCharacter)
	{
		PlayerController = Cast<ABaseCharacterPlayerController>(BaseCharacter->GetController());

		// Set Index to the selected weapon of the character
		for (int32 i = 0; i <= 7; i++)
		{
			if (BaseCharacter->GetSelectedWeaponType() == GetWeaponSlotOnIndex(i).WeaponType)
			{
				CurrentIndex = i;
				UpdateDynamicMaterialParameters();
				break;
			}
		}

		// Based on Character's Weapon Inventory update the 7 Weapon Slots
		for (int32 i = 1; i <= 7; i++)
		{
			AWeapon* Weapon = Cast<AWeapon>(BaseCharacter->GetWeaponInInventory(GetWeaponSlotOnIndex(i).WeaponType));
			if (Weapon)
			{
				GetWeaponSlotOnIndex(i).WeaponImage->SetBrushFromTexture(Weapon->GetWeaponIconTexture());
				GetWeaponSlotOnIndex(i).WeaponName->SetText(FText::FromString(FString::FromInt(BaseCharacter->GetTotalAmmoAmount(Weapon->GetAmmoType()))));
			}
			else
			{
				GetWeaponSlotOnIndex(i).WeaponImage->SetOpacity(0.0f);
				GetWeaponSlotOnIndex(i).WeaponName->SetOpacity(0.0f);
			}
		}
	}
}

void UInventoryWheelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Update Current Input based on Joystick or Mouse
	UpdateCurrentInput();

	// Calculate Index based on CurrentInput
	UpdateIndex();

	// Update selected weapon type
	BaseCharacter->SetSelectedWeaponType(GetWeaponSlotOnIndex(CurrentIndex).WeaponType);

	// Update Weapon Name
	WeaponNameText->SetOpacity(0.0f);
	if (CurrentIndex != 0)
	{
		AWeapon* Weapon = Cast<AWeapon>(BaseCharacter->GetWeaponInInventory(GetWeaponSlotOnIndex(CurrentIndex).WeaponType));
		if (Weapon)
		{
			WeaponNameText->SetOpacity(1.0f);
			WeaponNameText->SetText(FText::FromString(Weapon->GetItemName()));
		}
	}
	else
	{
		WeaponNameText->SetOpacity(1.0f);
		WeaponNameText->SetText(FText::FromString("Unarmed"));
	}
}

void UInventoryWheelWidget::InitializeTMap()
{
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

	IndexToWeaponSlot.Add(0, FWeaponSlot(EWeaponType::Unarmed,			nullptr, nullptr));
	IndexToWeaponSlot.Add(1, FWeaponSlot(EWeaponType::Shotgun,			Index1Image, Index1Text));
	IndexToWeaponSlot.Add(2, FWeaponSlot(EWeaponType::Pistol,			Index2Image, Index2Text));
	IndexToWeaponSlot.Add(3, FWeaponSlot(EWeaponType::RocketLauncher,	Index3Image, Index3Text));
	IndexToWeaponSlot.Add(4, FWeaponSlot(EWeaponType::Sniper,			Index4Image, Index4Text));
	IndexToWeaponSlot.Add(5, FWeaponSlot(EWeaponType::GrenadeLauncher,	Index5Image, Index5Text));
	IndexToWeaponSlot.Add(6, FWeaponSlot(EWeaponType::AssaultRifle,		Index6Image, Index6Text));
	IndexToWeaponSlot.Add(7, FWeaponSlot(EWeaponType::SMG,				Index7Image, Index7Text));
}

float UInventoryWheelWidget::CalculateSegmentDegreeSize() const
{
	return 360.0f / static_cast<float>(Segments);
}

void UInventoryWheelWidget::UpdateCurrentInput()
{
	if (PlayerController->GetInputType() == EInputType::Controller)
	{
		UpdateDirectionWithJoystick();
	}
	else if (PlayerController->GetInputType() == EInputType::KeyboardMouse)
	{
		UpdateDirectionWithMouse();
	}
}

void UInventoryWheelWidget::UpdateDirectionWithJoystick()
{
	CurrentInput = PlayerController->GetGamepadInput();
	CurrentInput.Y *= -1.0f;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(21, 0, FColor::Black, FString::Printf(TEXT("FROM DEAD ZONE : %f"), CurrentInput.Size()), false);
	}

	if (CurrentInput.Size() <= JoystickDeadzone)
	{
		CurrentInput = FVector2D(0.0f, 0.0f);
	}
}

void UInventoryWheelWidget::UpdateDirectionWithMouse()
{
	float MouseX, MouseY;
	PlayerController->GetMousePosition(MouseX, MouseY);

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	CurrentInput.X = MouseX - FMath::FloorToFloat(ViewportSize.X / 2.0f);
	CurrentInput.Y = MouseY - FMath::FloorToFloat(ViewportSize.Y / 2.0f);
	CurrentInput.Y *= -1.0f;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(8, 0, FColor::Black, FString::Printf(TEXT("Viewport : %f  %f"), ViewportSize.X, ViewportSize.Y), false);
		GEngine->AddOnScreenDebugMessage(6, 0, FColor::Black, FString::Printf(TEXT("Mouse Position : %f  %f"), MouseX, MouseY), false);
		GEngine->AddOnScreenDebugMessage(7, 0, FColor::Black, FString::Printf(TEXT("Mouse Position Offset : %f  %f"), CurrentInput.X, CurrentInput.Y), false);
		GEngine->AddOnScreenDebugMessage(20, 0, FColor::Black, FString::Printf(TEXT("FROM DEAD ZONE : %f"), CurrentInput.Size()), false);
	}

	if (CurrentInput.Size() <= MouseCursorDeadzone)
	{
		CurrentInput = FVector2D(0.0f, 0.0f);
	}
}

void UInventoryWheelWidget::UpdateIndex()
{
	if (CurrentInput == FVector2D::ZeroVector) return;

	CurrentInput.Normalize();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(3, 0, FColor::Black, FString::Printf(TEXT("CurrentInput : %f  %f"), CurrentInput.X, CurrentInput.Y), false);
	}

	CurrentInput = CurrentInput.GetRotated(180.0f);
	float Angle = FMath::RadiansToDegrees(UKismetMathLibrary::Atan2(CurrentInput.X, CurrentInput.Y) + UKismetMathLibrary::GetPI());
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(4, 0, FColor::Black, FString::Printf(TEXT("Angle : %f"), Angle), false);
	}

	CurrentIndex = FMath::RoundToFloat(Angle / CalculateSegmentDegreeSize());
	if (static_cast<int32>(CurrentIndex) == Segments)
	{
		CurrentIndex = 0;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(5, 0, FColor::Black, FString::Printf(TEXT("Index : %f"), CurrentIndex), false);
	}

	UpdateDynamicMaterialParameters();
}

void UInventoryWheelWidget::UpdateDynamicMaterialParameters()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(FName("Index"), CurrentIndex);
	}
}

