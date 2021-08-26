#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AssaultRifle		UMETA(DisplayName = "AssaultRifle"),
	Pistol				UMETA(DisplayName = "Pistol"),
	Shotgun				UMETA(DisplayName = "Shotgun"),
	Sniper				UMETA(DisplayName = "Sniper"),
	GrenadeLauncher		UMETA(DisplayName = "GrenadeLauncher"),
	RocketLauncher		UMETA(DisplayName = "RocketLauncher"),
	SMG					UMETA(DisplayName = "SMG"),

	Unarmed				UMETA(DisplayName = "Unarmed")
};

