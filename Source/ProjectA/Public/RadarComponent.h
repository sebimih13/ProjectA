// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Headers/MarkerType.h"
#include "RadarComponent.generated.h"

/** Forward Declarations */
class ABaseCharacter;
class URadarWidget;
class ALandmarkTarget;

USTRUCT(BlueprintType)
struct FDirection
{
	GENERATED_BODY()

public:
	FDirection() { };
	FDirection(FString DesiredName, int32 DesiredWorldDirection)
	{
		Name = DesiredName;
		WorldDirection = DesiredWorldDirection;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Direction")
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Direction")
	int32 WorldDirection;
};

USTRUCT(BlueprintType)
struct FMarkerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuestMarkers")
	EMarkerType Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuestMarkers")
	FVector Location;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTA_API URadarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URadarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** References */
	ABaseCharacter* BaseCharacter;
	URadarWidget* RadarWidget;

	/** Visibility */
	bool bVisibile = true;
	bool bCanChangeVisibility = true;

	FTimerHandle ResetCanChangeVisibilityTimer;

	/** Quest Markers */
	TArray<FMarkerInfo> QuestMarkers;

	/** Landmarks */
	TArray<ALandmarkTarget*> Landmarks;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TArray<FDirection> Directions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TArray<FMarkerInfo> DefaultQuestMarkers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float UnitsPerMeter = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<URadarWidget> RadarWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float MaxWidgetTranslation = 343.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<ALandmarkTarget> LandmarkTargetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	bool bHideOutOfSightMarkers = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float DefaultElevationRange = 150.0f;

private:
	/** Calculate Functions */
	float CalculateRotationToCircleDegrees(float Rotation) const;
	float CalculateDeltaClockwise(float A, float B, bool bClockwise) const;
	bool InRadarSight(FRotator A, FRotator B, FVector2D& Translation, bool& bClockwise);

	/** Visibility */
	void ResetCanChangeVisibility();
	void SetVisibility(bool bVisibility, bool bAnimate);

	/** Directions */
	void InitializeDirections();
	void UpdateDirectionWidgets();

	/** Quest Markers */
	void InitializeQuestMarkers();
	void AddMarkerToRadar(FMarkerInfo Info);
	void UpdateMarkersDistance();
	void UpdateMarkersPosition();
	void UpdateMarkersElevation();

	bool RemoveQuestMarker(FMarkerInfo MarkerToRemove);
	bool RemoveQuestMarkerAtIndex(int32 Index);

	/** Landmarks */
	void InitializeLandmarks();
	void AddLandmarkToRadar(ALandmarkTarget* Landmark);
	void UpdateLandmarksPosition();

public:
	void InitializeRadar(ABaseCharacter* Character);

	void ToggleVisibility(bool bAnimate);

	/** Update based on Player Actions */
	void OnPlayerTurned();
	void OnPlayerMoved();

	void OnBecomeVisible();
};

