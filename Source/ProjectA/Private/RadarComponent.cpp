// Fill out your copyright notice in the Description page of Project Settings.

#include "RadarComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "RadarWidget.h"
#include "RadarDirectionWidget.h"
#include "RadarMarkerWidget.h"
#include "RadarLandmarkWidget.h"

#include "LandmarkTarget.h"

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
			InitializeDirections();
			InitializeQuestMarkers();
			InitializeLandmarks();

			OnBecomeVisible();

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

bool URadarComponent::InRadarSight(FRotator A, FRotator B, FVector2D& Translation, bool& bClockwise)
{
	float ADegrees = CalculateRotationToCircleDegrees(A.Yaw);
	float BDegrees = CalculateRotationToCircleDegrees(B.Yaw);

	float DeltaClockwise = CalculateDeltaClockwise(ADegrees, BDegrees, true);
	float DeltaAnticlockwise = CalculateDeltaClockwise(ADegrees, BDegrees, false);

	bClockwise = (DeltaAnticlockwise >= DeltaClockwise);

	float Percent = (bClockwise ? DeltaClockwise : DeltaAnticlockwise) / BaseCharacter->GetFollowCamera()->FieldOfView;
	if (Percent <= 1.0f)
	{
		Translation = FVector2D(FMath::Lerp(0.0f, MaxWidgetTranslation, Percent) * (bClockwise ? 1.0f : -1.0f), 0.0f);
	}

	return Percent <= 1.0f;
}

// Visibility
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
				OnBecomeVisible();
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
			if (bVisibile)
			{
				OnBecomeVisible();
			}

			RadarWidget->SetVisibility(bVisibile ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
			bCanChangeVisibility = true;
		}
	}
}

void URadarComponent::ToggleVisibility(bool bAnimate)
{
	SetVisibility(!bVisibile, bAnimate);
}

// Direction Widgets
void URadarComponent::InitializeDirections()
{
	for (FDirection Dir : Directions)
	{
		RadarWidget->AddDirection(Dir.Name, Dir.WorldDirection);
	}
}

void URadarComponent::UpdateDirectionWidgets()
{
	for (URadarDirectionWidget* DirWidget : RadarWidget->GetDirectionWidgets())
	{
		FVector2D Translation = FVector2D::ZeroVector;
		bool bClockwise = false;
		bool bInRadarSight = InRadarSight(BaseCharacter->GetFollowCamera()->GetComponentRotation(), FRotator(0.0f, static_cast<float>(DirWidget->WorldRotation), 0.0f), Translation, bClockwise);

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

// Quest Markers
void URadarComponent::InitializeQuestMarkers()
{
	for (FMarkerInfo Marker : DefaultQuestMarkers)
	{
		AddMarkerToRadar(Marker);
	}
}

void URadarComponent::AddMarkerToRadar(FMarkerInfo Info)
{
	QuestMarkers.Add(Info);
	RadarWidget->AddMarker(Info.Type);
}

void URadarComponent::UpdateMarkersDistance()
{
	int32 Index = 0;
	for (URadarMarkerWidget* MarkerWidget : RadarWidget->GetMarkerWidgets())
	{
		float Distance = (QuestMarkers[Index++].Location - BaseCharacter->GetActorLocation()).Size2D() / UnitsPerMeter;
		MarkerWidget->UpdateDistance(FMath::RoundToInt(Distance));
	}
}

void URadarComponent::UpdateMarkersPosition()
{
	int32 Index = 0;
	for (URadarMarkerWidget* MarkerWidget : RadarWidget->GetMarkerWidgets())
	{
		FVector2D Translation = FVector2D::ZeroVector;
		bool bClockwise = false;
		bool bInRadarSight = InRadarSight(BaseCharacter->GetFollowCamera()->GetComponentRotation(), UKismetMathLibrary::FindLookAtRotation(BaseCharacter->GetActorLocation(), QuestMarkers[Index++].Location), Translation, bClockwise);

		if (bInRadarSight)
		{
			MarkerWidget->SetRenderTranslation(Translation);

			if (!MarkerWidget->IsVisible())
			{
				MarkerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		else if (!bHideOutOfSightMarkers)
		{
			Translation = FVector2D(MaxWidgetTranslation * (bClockwise ? 1.0f : -1.0f), 0.0f);
			MarkerWidget->SetRenderTranslation(Translation);
		}
		else if (MarkerWidget->IsVisible())
		{
			MarkerWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void URadarComponent::UpdateMarkersElevation()
{
	int32 Index = 0;
	for (URadarMarkerWidget* MarkerWidget : RadarWidget->GetMarkerWidgets())
	{
		if (QuestMarkers[Index].Location.Z > BaseCharacter->GetActorLocation().Z + DefaultElevationRange)
		{
			MarkerWidget->SetArrowDirection(EArrowDirection::Up);
		}
		else if (QuestMarkers[Index].Location.Z < BaseCharacter->GetActorLocation().Z - DefaultElevationRange)
		{
			MarkerWidget->SetArrowDirection(EArrowDirection::Down);
		}
		else
		{
			MarkerWidget->SetArrowDirection(EArrowDirection::None);
		}
		Index++;
	}
}

bool URadarComponent::RemoveQuestMarker(FMarkerInfo MarkerToRemove)
{
	int32 Index = 0;
	for (Index = 0; Index < QuestMarkers.Num(); Index++)
	{
		if (QuestMarkers[Index].Type == MarkerToRemove.Type && QuestMarkers[Index].Location == MarkerToRemove.Location)
		{
			break;
		}
	}

	if (Index != QuestMarkers.Num())
	{
		QuestMarkers.RemoveAt(Index);
		RadarWidget->GetMarkerWidgets()[Index]->RemoveFromParent();
		RadarWidget->GetMarkerWidgets().RemoveAt(Index);
		return true;
	}
	return false;
}

bool URadarComponent::RemoveQuestMarkerAtIndex(int32 Index)
{
	if (Index < QuestMarkers.Num() && QuestMarkers.Num() > 0)
	{
		RemoveQuestMarker(QuestMarkers[Index]);
	}
	return false;
}

// Landmarks
void URadarComponent::InitializeLandmarks()
{
	TArray<AActor*> LandmarkTargets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), LandmarkTargetClass, LandmarkTargets);

	for (AActor* LandmarkTarget : LandmarkTargets)
	{
		ALandmarkTarget* Target = Cast<ALandmarkTarget>(LandmarkTarget);
		if (Target)
		{
			AddLandmarkToRadar(Target);
		}
	}
}

void URadarComponent::AddLandmarkToRadar(ALandmarkTarget* Landmark)
{
	if (!Landmarks.Contains(Landmark))
	{
		Landmarks.Add(Landmark);
		RadarWidget->AddLandmark(Landmark->IconOnRadar);
	}
}

void URadarComponent::UpdateLandmarksPosition()
{
	int32 Index = 0;
	for (URadarLandmarkWidget* LandmarkWidget : RadarWidget->GetLandmarkWidgets())
	{
		FVector2D Translation = FVector2D::ZeroVector;
		bool bClockwise = false;
		bool bInRadarSight = InRadarSight(BaseCharacter->GetFollowCamera()->GetComponentRotation(), UKismetMathLibrary::FindLookAtRotation(BaseCharacter->GetActorLocation(), Landmarks[Index++]->GetActorLocation()), Translation, bClockwise);

		if (bInRadarSight)
		{
			LandmarkWidget->SetRenderTranslation(Translation);

			if (!LandmarkWidget->IsVisible())
			{
				LandmarkWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		else if (LandmarkWidget->IsVisible())
		{
			LandmarkWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// On Player
void URadarComponent::OnPlayerTurned()
{
	if (bVisibile)
	{
		UpdateDirectionWidgets();
		UpdateMarkersPosition();
		UpdateLandmarksPosition();
	}
}

void URadarComponent::OnPlayerMoved()
{
	if (bVisibile)
	{
		UpdateMarkersDistance();
		UpdateMarkersPosition();
		UpdateMarkersElevation();
		UpdateLandmarksPosition();
	}
}

void URadarComponent::OnBecomeVisible()
{
	UpdateDirectionWidgets();
	UpdateMarkersDistance();
	UpdateMarkersPosition();
	UpdateMarkersElevation();
	UpdateLandmarksPosition();
}

