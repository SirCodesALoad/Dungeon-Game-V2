#pragma once
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "DungeonCrawler/DungeonCrawlerCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "CastTimeAbility.generated.h"

USTRUCT(BlueprintType)
struct FAbilityCastMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category= "GameplayMessage")
	FText AbilityName = FText();

	UPROPERTY(BlueprintReadWrite, Category= "GameplayMessage")
	float CastDuration = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category= "GameplayMessage")
	TWeakObjectPtr<const ADungeonCrawlerCharacter> ActorReporting;

	// Returns a debug string representation of this message
	DUNGEONCRAWLER_API FString ToString() const;
};

UCLASS(Blueprintable)
class UCastTimeAbility : public UGameplayAbility 
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category= "Ability Info")
	FText AbilityName = FText();

	UPROPERTY(BlueprintReadWrite, Category= "Ability Info")
	float CastDuration = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category= "Ability Info")
	UAnimMontage* MontageToPlay;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CastTimeAbility")
	UAbilityTask_WaitDelay* WaitDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CastTimeAbility")
	FName  EndCastTimeSectionName = "CastTimeEnded";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CastTimeAbility")
	FGameplayTag MessageTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CastTime")
	float CastTime = 1.f;
	
	UCastTimeAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);
	
	UFUNCTION(BlueprintCallable, Category = "CastTimeAbility")
	void StartCasting();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCastingStarted();
	
	UFUNCTION()
	void OnFinishCasting();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnCastingSucceded();
	
	UFUNCTION(BlueprintCallable, Category = "CastTimeAbility")
	float GetSectionStartTime(const FName SectionName, const float PlayRate = 1.f) const;
	
};
