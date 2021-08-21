// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveVector.h"

#include "BaseCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetSphereRadius(200.0f);

	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Set Custom Depth to disable
	InitializeCustomDepth();
	
	// Hide Pickup Widget
	PickupWidgetComponent->SetVisibility(false);

	// Setup overlap for Area Sphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set Item Properties based on Item State
	SetItemProperties();

	if (GetItemState() == EItemState::Pickup)
	{
		StartPulseTimer();
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterping(DeltaTime);
	UpdatePulse();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
	}	
	EnableGlowMaterial();
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);

		if (BaseCharacter)
		{
			BaseCharacter->IncrementOverlappedItemsCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);

		if (BaseCharacter)
		{
			BaseCharacter->IncrementOverlappedItemsCount(-1);
		}
	}
}

void AItem::SetItemState(EItemState DesiredState)
{
	ItemState = DesiredState;
	SetItemProperties();
}

void AItem::SetItemProperties()
{
	switch (ItemState)
	{
	case EItemState::Pickup:
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::Equipped:
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PickupWidgetComponent->SetVisibility(false);
		break;

	case EItemState::Falling:
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EquipInterping:
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PickupWidgetComponent->SetVisibility(false);
		break;
	}
}

void AItem::StartItemCurve(ABaseCharacter* BaseCharacter)
{
	Character = BaseCharacter;
	SetItemState(EItemState::EquipInterping);
	GetWorldTimerManager().ClearTimer(PulseTimer);

	PlayPickupSound();

	// Add 1 for the ItemCount for this InterLocation struct
	InterpLocationIndex = Character->GetInterpLocationIndex();
	Character->IncrementInterpLocationItemCount(InterpLocationIndex, 1);

	bInterping = true;
	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	const float CameraRotationYaw = Character->GetFollowCamera()->GetComponentRotation().Yaw;
	const float ItemRotatationYaw = GetActorRotation().Yaw;
	InterpInitialYawOffset = ItemRotatationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = false;
}

void AItem::FinishInterping()
{
	if (Character)
	{
		PlayEquipSound();
		Character->GetPickupItem(this);
		Character->IncrementInterpLocationItemCount(InterpLocationIndex, -1);
		SetItemState(EItemState::PickedUp);
	}

	// Set scale back to normal
	SetActorScale3D(FVector(1.0f));

	// Disable Glow Material + Custom Depth
	DisableGlowMaterial();
	bCanChangeCustomDepth = true;
	DisableCustomDepth();

	bInterping = false;
	GetWorldTimerManager().ClearTimer(ItemInterpTimer);
}

void AItem::ItemInterping(float DeltaTime)
{
	if (!bInterping) return;

	if (Character && CurveItemZ)
	{
		// Elapsed Time since we started ItemInterpTimer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		const float CurveValue = CurveItemZ->GetFloatValue(ElapsedTime);

		FVector ItemLocation = GetActorLocation();
		const FVector& CameraInterpLocation = GetInterpLocation();

		// Scale factor to multiply with Curve Value
		const float DeltaZ = FVector(0.0f, 0.0f, (CameraInterpLocation - ItemLocation).Z).Z;

		// Interpolate Z value
		ItemLocation.Z += CurveValue * DeltaZ;

		// Interpolate X and Y values
		ItemLocation.X = FMath::FInterpTo(ItemLocation.X, CameraInterpLocation.X, DeltaTime, 30.0f);
		ItemLocation.Y = FMath::FInterpTo(ItemLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.0f);

		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// Set Rotation (don't rotate with the camera)
		const FRotator& CameraRoation = Character->GetFollowCamera()->GetComponentRotation();
		FRotator ItemRotation = FRotator(0.0f, CameraRoation.Yaw + InterpInitialYawOffset, 0.0f);

		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		// Scale Item
		if (CurveItemScale)
		{
			const float ScaleCurveValue = CurveItemScale->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if (!Character) return FVector::ZeroVector;

	switch (ItemType)
	{
	case EItemType::Ammo:	return Character->GetInterpLocation(InterpLocationIndex);	break;
	case EItemType::Weapon:	return Character->GetInterpLocation(0);						break;
	}

	return FVector::ZeroVector;
}

void AItem::PlayPickupSound()
{
	if (Character && Character->GetShouldPlayPickupSound())
	{
		Character->StartPickupSoundTimer();
		if (PickupSound)
		{
			UGameplayStatics::PlaySound2D(this, PickupSound);
		}
	}
}

void AItem::PlayEquipSound()
{
	if (Character && Character->GetShouldPlayEquipSound())
	{
		Character->StartEquipSoundTimer();
		if (EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, EquipSound);
		}
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::EnableCustomDepth()
{

}

void AItem::DisableCustomDepth()
{

}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0.0f);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1.0f);
	}
}

void AItem::StartPulseTimer()
{
	if (GetItemState() == EItemState::Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::UpdatePulse()
{
	FVector CurveValue;
	
	switch (GetItemState())
	{
	case EItemState::Pickup:
		if (CurvePulse)
		{
			const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
			CurveValue = CurvePulse->GetVectorValue(ElapsedTime);
		}
		break;

	case EItemState::EquipInterping:
		if (CurveInterpPulse)
		{
			const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = CurveInterpPulse->GetVectorValue(ElapsedTime);
		}
		break;
	}

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(FName("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(FName("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(FName("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
}

