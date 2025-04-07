// Fill out your copyright notice in the Description page of Project Settings.

#include "UTankAttributeSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AbilitySystemComponent.h"
#include "ChangeHealthUIDisplayMessage.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"

UTankAttributeSet::UTankAttributeSet()
{
}

// void UTankAttributeSet::DamageChangeDelegate(const FOnAttributeChangeData& OnAttributeChangeData)
// {
// 	float LocalDamageDone = OnAttributeChangeData.NewValue;
//
// 	if (LocalDamageDone > 0.0f)
// 	{
// 		const float CurrShieldHealth = GetShieldHealth();
// 		if (CurrShieldHealth > 0.0f && OnAttributeChangeData.GEModData->Target.HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("ClassUnique.IsBlocking"))))
// 		{
// 			const float NewShieldHealth = CurrShieldHealth - LocalDamageDone;
// 			LocalDamageDone =- CurrShieldHealth;
// 			SetShieldHealth(FMath::Clamp(NewShieldHealth, 0.0f, GetShieldHealthMax()));
// 		}
// 	}
//
// 	OnAttributeChangeData.Attribute.SetNumericValueChecked(LocalDamageDone, nullptr);
// 	
// 	Super::DamageChangeDelegate(OnAttributeChangeData);
// }

void UTankAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

//PreGameplayEffectExecute. PreAttributeChanged,

bool UTankAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UTankAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("PostGameplayEffectExecute"));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, Data.EvaluatedData.Attribute.AttributeName);
	}

	//array of function pointers populated based on the class.
	
	if (Data.EvaluatedData.Attribute.AttributeName == "Damage")
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute == Damage"));

		float LocalDamageDone = Data.EvaluatedData.Magnitude;

		if (LocalDamageDone > 0.0f)
		{
			const float CurrShieldHealth = GetShieldHealth();
			if (CurrShieldHealth > 0.0f && Data.Target.HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("ClassUnique.IsBlocking"))))
			{
				const float NewShieldHealth = CurrShieldHealth - LocalDamageDone;
				LocalDamageDone =- CurrShieldHealth;
				SetShieldHealth(FMath::Clamp(NewShieldHealth, 0.0f, GetShieldHealthMax()));
			}
		}
		if (LocalDamageDone < 0.0f)
		{
			LocalDamageDone = 0.0f;
		}
		Data.EvaluatedData.Magnitude = LocalDamageDone;
	}

	if (GetShieldHealth() >= GetShieldHealthMax() || Data.Target.HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("ClassUnique.IsBlocking"))))
	{
		const FGameplayTag RegenTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.RegenShieldHealth"));
		const FGameplayTagContainer& RegenTagContainer = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>({RegenTag}));
		Data.Target.CancelAbilities(&RegenTagContainer, nullptr, nullptr);
	}
	
	FGameplayTag ChannelTag = FGameplayTag::RequestGameplayTag(TEXT("Lyra.StatChange.ShieldHealth.Message")); 
	FChangeHealthUIDisplayMessage OutgoingMessage;
	const float CurrHealthValue = GetShieldHealth();
	const float MaxHealthValue = GetShieldHealthMax();
	OutgoingMessage.HealthPercentage = CurrHealthValue / MaxHealthValue;
	OutgoingMessage.HealthTextValue = FText::Format(FText::FromString(TEXT("{0}:{1}")), CurrHealthValue, MaxHealthValue);
	OutgoingMessage.ActorReporting = Cast<ADungeonCrawlerCharacter>(Data.Target.GetOwnerActor());
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSubsystem.BroadcastMessage(ChannelTag, OutgoingMessage);
	
	Super::PostGameplayEffectExecute(Data);
}

// void UTankAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME_CONDITION_NOTIFY(UStandardAttributeSet, Health, COND_None, REPNOTIFY_Always);
// }

