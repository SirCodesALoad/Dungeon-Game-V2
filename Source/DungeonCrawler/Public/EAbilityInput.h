#pragma once

UENUM(BlueprintType)
enum class EAbilityInput : uint8
{
	None = 0 UMETA(Hidden),
	Jump = 1,
	PrimaryAttack = 2,
	SecondaryAttack = 3,
	AbilityOne = 4,
	AbilityTwo = 5,
	DodgeRoll = 6
};
