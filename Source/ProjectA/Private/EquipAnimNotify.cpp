// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipAnimNotify.h"
#include "BaseCharacter.h"

void UEquipAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp && MeshComp->GetOwner())
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());

		if (Character)
		{
			Character->SetCombatState(ECombatState::Normal);
		}
	}
}

