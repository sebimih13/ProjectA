// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

/** Forward Declarations */

UCLASS()
class PROJECTA_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	UWeaponAnimInstance();

	/** Begin Play */
	virtual void NativeInitializeAnimation() override;

	/** Tick */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Clip")
	bool bMovingClip = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Clip")
	FTransform ClipTransform;
};

