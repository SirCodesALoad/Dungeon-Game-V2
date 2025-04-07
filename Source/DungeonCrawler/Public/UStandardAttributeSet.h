#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AttributeHelpers.h"
#include "AttributeSet.h"
#include "UStandardAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UStandardAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "HealthMax", ReplicatedUsing=OnRep_HealthMax)
	FGameplayAttributeData HealthMax;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, HealthMax);


	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, Damage);
	
	UPROPERTY(BlueprintReadOnly, Category = "DamageResistance")
	FGameplayAttributeData DamageResistance;
	ATTRIBUTE_ACCESSORS(UStandardAttributeSet, DamageResistance);

	UStandardAttributeSet();


	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};