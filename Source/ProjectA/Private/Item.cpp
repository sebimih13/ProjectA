// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"

#include "BaseCharacter.h"
#include "PickupWidget.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidgetComponent->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
	AreaSphere->SetSphereRadius(200.0f);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Hide Pickup Widget
	PickupWidgetComponent->SetVisibility(false);

	// Setup overlap for Area Sphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set Pickup Widget Reference to this class
	UPickupWidget* PickupWidget = Cast<UPickupWidget>(PickupWidgetComponent->GetUserWidgetObject());
	if (PickupWidget)
	{
		PickupWidget->Item = this;
	}

	// Set Item Properties based on Item State
	SetItemProperties();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterping(DeltaTime);
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
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::Equipped:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PickupWidgetComponent->SetVisibility(false);
		break;

	case EItemState::Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EquipInterping:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	bInterping = true;
	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	const float CameraRotationYaw = Character->GetFollowCamera()->GetComponentRotation().Yaw;
	const float ItemRotatationYaw = GetActorRotation().Yaw;
	InterpInitialYawOffset = ItemRotatationYaw - CameraRotationYaw;
}

void AItem::FinishInterping()
{
	if (Character)
	{
		Character->GetPickupItem(this);
	}

	// Set scale back to normal
	SetActorScale3D(FVector(1.0f));

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
		const FVector& CameraInterpLocation = Character->GetCameraInterpLocation();

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

