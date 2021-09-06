// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "RadarWidget.h"
#include "RadarDirectionWidget.h"

#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
URadarComponent::URadarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Directions.Add(FDirection("N", 0));
	Directions.Add(FDirection("E", 90));
	Directions.Add(FDirection("S", 180));
	Directions.Add(FDirection("W", -90));
}

// Called when the game starts
void URadarComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void URadarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URadarComponent::InitializeRadar(ABaseCharacter* Character)
{
	BaseCharacter = Character;

	if (RadarWidgetClass)
	{
		RadarWidget = CreateWidget<URadarWidget>(GetWorld(), RadarWidgetClass);

		if (RadarWidget)
		{
			for (FDirection Dir : Directions)
			{
				RadarWidget->AddDirection(Dir.Name, Dir.WorldDirection);
			}

			UpdateDirectionWidgets();
			RadarWidget->AddToViewport();
		}
	}
}

// Calculate Functions
float URadarComponent::CalculateRotationToCircleDegrees(float Rotation) const
{
	if (Rotation < 0.0f)
	{
		return UKismetMathLibrary::GenericPercent_FloatFloat(Rotation + 360.0f, 360.0f);
	}
	return Rotation;
}

float URadarComponent::CalculateDeltaClockwise(float A, float B, bool bClockwise) const
{
	A = UKismetMathLibrary::GenericPercent_FloatFloat(A, 360.0f);
	B = UKismetMathLibrary::GenericPercent_FloatFloat(B, 360.0f);

	if (bClockwise)
	{
		if (A > B)
		{
			return 360.0f - (A - B);
		}
		else
		{
			return B - A;
		}
	}
	else
	{
		if (A < B)
		{
			return 360.0f - (B - A);
		}
		else
		{
			return A - B;
		}
	}
}

bool URadarComponent::InRadarSight(FRotator A, FRotator B, FVector2D& Translation)
{
	float ADegrees = CalculateRotationToCircleDegrees(A.Yaw);
	float BDegrees = CalculateRotationToCircleDegrees(B.Yaw);

	float DeltaClockwise = CalculateDeltaClockwise(ADegrees, BDegrees, true);
	float DeltaAnticlockwise = CalculateDeltaClockwise(ADegrees, BDegrees, false);

	bool bClockwise = (DeltaAnticlockwise >= DeltaClockwise);

	float Percent = (bClockwise ? DeltaClockwise : DeltaAnticlockwise) / BaseCharacter->GetFollowCamera()->FieldOfView;
	if (Percent <= 1.0f)
	{
		Translation = FVector2D(FMath::Lerp(0.0f, MaxWidgetTranslation, Percent) * (bClockwise ? 1.0f : -1.0f), 0.0f);
	}

	return Percent <= 1.0f;
}

void URadarComponent::ResetCanChangeVisibility()
{
	if (!bVisibile)
	{
		RadarWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	bCanChangeVisibility = true;
	GetWorld()->GetTimerManager().ClearTimer(ResetCanChangeVisibilityTimer);
}

void URadarComponent::SetVisibility(bool bVisibility, bool bAnimate)
{
	if (bVisibile != bVisibility && bCanChangeVisibility)
	{
		bCanChangeVisibility = false;
		bVisibile = bVisibility;

		if (bAnimate)
		{
			if (bVisibile)
			{
				RadarWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				RadarWidget->PlayAnimation(RadarWidget->BlendOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Reverse);
				GetWorld()->GetTimerManager().SetTimer(ResetCanChangeVisibilityTimer, this, &URadarComponent::ResetCanChangeVisibility, RadarWidget->BlendOutAnimation->GetEndTime());
			}
			else
			{
				RadarWidget->PlayAnimation(RadarWidget->BlendOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
				GetWorld()->GetTimerManager().SetTimer(ResetCanChangeVisibilityTimer, this, &URadarComponent::ResetCanChangeVisibility, RadarWidget->BlendOutAnimation->GetEndTime());
			}
		}
		else
		{
			RadarWidget->SetVisibility(bVisibile ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
			bCanChangeVisibility = true;
		}
	}
}

void URadarComponent::ToggleVisibility(bool bAnimate)
{
	SetVisibility(!bVisibile, bAnimate);
}

void URadarComponent::UpdateDirectionWidgets()
{
	for (URadarDirectionWidget* DirWidget : RadarWidget->GetDirectionWidgets())
	{
		FVector2D Translation = FVector2D::ZeroVector;
		bool bInRadarSight = InRadarSight(BaseCharacter->GetFollowCamera()->GetComponentRotation(), FRotator(0.0f, static_cast<float>(DirWidget->WorldRotation), 0.0f), Translation);

		if (bInRadarSight)
		{
			DirWidget->SetRenderTranslation(Translation);

			if (!DirWidget->IsVisible())
			{
				DirWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		else if (DirWidget->IsVisible())
		{
			DirWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URadarComponent::OnPlayerTurned()
{
	if (bVisibile)
	{
		UpdateDirectionWidgets();
	}
}

