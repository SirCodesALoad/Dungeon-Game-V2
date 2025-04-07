// Fill out your copyright notice in the Description page of Project Settings.


#include "UStandardAttributeSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystemComponent.h"
#include "ChangeHealthUIDisplayMessage.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "UTankAttributeSet.h"
#include "Net/UnrealNetwork.h"

UStandardAttributeSet::UStandardAttributeSet()
{
	Health = 100;
}

void UStandardAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetHealthMax());
	}
	
}

//PreGameplayEffectExecute. PreAttributeChanged,

void UStandardAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// auto TankAttributes =  Data.Target.GetSet<UTankAttributeSet>();
	// if(TankAttributes != nullptr)
	// {
	// 	TankAttributes->get
	// }
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float LocalDamageDone = Data.EvaluatedData.Magnitude;
		SetDamage(0.f);

		UE_LOG(LogTemp, Warning, TEXT("Character has Just been damaged"));
		if (LocalDamageDone > 0.0f)
		{
			if (LocalDamageDone > 0.0f)
			{
				const float NewHealth = GetHealth() - LocalDamageDone;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetHealthMax()));
				
			}
		}
	} 
}

void UStandardAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, Health, OldHealth);
}

void UStandardAttributeSet::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UStandardAttributeSet, HealthMax, OldHealthMax);
}

void UStandardAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

