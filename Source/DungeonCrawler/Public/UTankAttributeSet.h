#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AttributeHelpers.h"
#include "AttributeSet.h"
#include "UStandardAttributeSet.h"
#include "UTankAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UTankAttributeSet : public UStandardAttributeSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "ShieldHealth")
	FGameplayAttributeData ShieldHealth;
	ATTRIBUTE_ACCESSORS(UTankAttributeSet, ShieldHealth);

	UPROPERTY(BlueprintReadOnly, Category = "ShieldHealthMax")
	FGameplayAttributeData ShieldHealthMax;
	ATTRIBUTE_ACCESSORS(UTankAttributeSet, ShieldHealthMax);
	
	UPROPERTY(BlueprintReadOnly, Category = "ShieldHealthRegenRate")
	FGameplayAttributeData ShieldHealthRegenRate;
	ATTRIBUTE_ACCESSORS(UTankAttributeSet, ShieldHealthRegenRate);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayAbility> 	RechargeShieldAbility = ConstructorHelpers::FClassFinder<UGameplayAbility>(TEXT("/Game/Gameplay/Abilities/GA_TankRechargeShieldHealth")).Class;

	UTankAttributeSet();
	
	//virtual void DamageChangeDelegate(const FOnAttributeChangeData& OnAttributeChangeData) override;

	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData &Data) override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
};