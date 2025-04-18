#include "CastTimeAbility.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Windows/WindowsTextInputMethodSystem.h"

UCastTimeAbility::UCastTimeAbility(): WaitDelay(nullptr)
{
	MessageTag = FGameplayTag::RequestGameplayTag("Lyra.Ability.Casting.Message");
}

void UCastTimeAbility::HandleMovementGameplayTagChanged(const FGameplayTag Tag, int32 Count)
{
	if (Tag == FGameplayTag::RequestGameplayTag(FName("Character.Moving")) && Count > 0 && IsInterruptible)
	{
		// End the ability when the movement tag is detected
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		UE_LOG(LogTemp, Warning, TEXT("Cast Time Ability interrupted due to movement!"));
	}
}


void UCastTimeAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}


void UCastTimeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	StartCasting();

	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (AbilitySystemComponent)
	{
		// Listen for the Gameplay Tag being applied
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Character.Moving")), EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &UCastTimeAbility::HandleMovementGameplayTagChanged);
	}

}


void UCastTimeAbility::StartCasting()
{
	//CastTime = GetSectionStartTime(EndCastTimeSectionName, 1.0f);
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	if (MessageSubsystem.IsValidLowLevelFast())
	{
		FAbilityCastMessage Message;
		Message.AbilityName = AbilityName;
		Message.CastDuration = CastTime;
		MessageSubsystem.BroadcastMessage(MessageTag, Message);
	}

	WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, CastTime);
	WaitDelay->OnFinish.AddDynamic(this, &UCastTimeAbility::OnFinishCasting);
	WaitDelay->ReadyForActivation();
	UE_LOG(LogTemp, Warning, TEXT("CastTime: %f"), CastTime);


	OnCastingStarted();
}

void UCastTimeAbility::OnFinishCasting()
{
	OnCastingSucceded();
}

void UCastTimeAbility::OnCastingSucceded_Implementation()
{
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}


float UCastTimeAbility::GetSectionStartTime(const FName SectionName, const float PlayRate) const
{
	if (MontageToPlay)
	{
		for (const FCompositeSection& Section : MontageToPlay->CompositeSections)
		{
			if (Section.SectionName == SectionName)
			{
				return Section.GetTime(static_cast<EAnimLinkMethod::Type>(EAnimLinkMethod::Type::Absolute / PlayRate));
			}
		}
	}
	return -1.0f;
}

void UCastTimeAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Remove the Gameplay Tag listener, clean up.
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Character.Moving")), EGameplayTagEventType::NewOrRemoved)
			.RemoveAll(this);
	}
}
