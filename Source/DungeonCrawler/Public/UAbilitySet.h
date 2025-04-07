#pragma once
#include "EAbilityInput.h"
#include "UStandardAttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "UAbilitySet.generated.h"

USTRUCT()
struct FAbilitySetItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	EAbilityInput InputKey;
};

UCLASS()
class DUNGEONCRAWLER_API UAbilitySet : public  UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<UStandardAttributeSet> ClassAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FAbilitySetItem> AbilitySetItems;

	TArray<FGameplayAbilitySpecHandle> GrantAbilitiesToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent) const;
};
