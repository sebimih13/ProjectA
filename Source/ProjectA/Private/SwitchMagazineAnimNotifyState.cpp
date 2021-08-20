// Fill out your copyright notice in the Description page of Project Settings.

#include "SwitchMagazineAnimNotifyState.h"
#include "BaseCharacter.h"
#include "Weapon.h"
#include "WeaponAnimInstance.h"

void USwitchMagazineAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		int32 ClipBoneIndex = Character->GetWeapon()->GetWeaponMesh()->GetBoneIndex(FName("Clip_Bone"));
		Character->GetLeftHandSceneComponent()->SetWorldTransform(Character->GetWeapon()->GetWeaponMesh()->GetBoneTransform(ClipBoneIndex));

		UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Character->GetWeapon()->GetWeaponMesh()->GetAnimInstance());
		if (WeaponAnimInstance)
		{
			WeaponAnimInstance->bMovingClip = true;
			WeaponAnimInstance->ClipTransform = Character->GetLeftHandSceneComponent()->GetComponentTransform();
		}
	}
}

void USwitchMagazineAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Character->GetWeapon()->GetWeaponMesh()->GetAnimInstance());
		if (WeaponAnimInstance)
		{
			WeaponAnimInstance->bMovingClip = true;
			WeaponAnimInstance->ClipTransform = Character->GetLeftHandSceneComponent()->GetComponentTransform();
		}
	}
}

void USwitchMagazineAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Character->GetWeapon()->GetWeaponMesh()->GetAnimInstance());
		if (WeaponAnimInstance)
		{
			WeaponAnimInstance->bMovingClip = false;
		}
	}
}

