// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponAnimInstance.h"

UWeaponAnimInstance::UWeaponAnimInstance()
{

}

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Don't run in editor
	if (DeltaSeconds == 0.0f)
	{
		return;
	}
}

