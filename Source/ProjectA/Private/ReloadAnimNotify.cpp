// Fill out your copyright notice in the Description page of Project Settings.

#include "ReloadAnimNotify.h"
#include "BaseCharacter.h"

void UReloadAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp && MeshComp->GetOwner())
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());

		if (Character)
		{
			Character->FinishReloading();
		}
	}
}

