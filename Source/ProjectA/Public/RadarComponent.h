// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RadarComponent.generated.h"

/** Forward Declarations */
class ABaseCharacter;
class URadarWidget;

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

	bool bVisibile = true;
	bool bCanChangeVisibility = true;

	FTimerHandle ResetCanChangeVisibilityTimer;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TArray<FDirection> Directions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	TSubclassOf<URadarWidget> RadarWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration")
	float MaxWidgetTranslation = 343.0f;

private:
	/** Calculate Functions */
	float CalculateRotationToCircleDegrees(float Rotation) const;
	float CalculateDeltaClockwise(float A, float B, bool bClockwise) const;
	bool InRadarSight(FRotator A, FRotator B, FVector2D& Translation);

	void ResetCanChangeVisibility();

	void UpdateDirectionWidgets();

	void SetVisibility(bool bVisibility, bool bAnimate);

public:
	void InitializeRadar(ABaseCharacter* Character);

	void ToggleVisibility(bool bAnimate);

	void OnPlayerTurned();
};

