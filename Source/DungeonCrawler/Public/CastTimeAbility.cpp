#include "CastTimeAbility.h"

#include "Windows/WindowsTextInputMethodSystem.h"

UCastTimeAbility::UCastTimeAbility(): WaitDelay(nullptr)
{
	MessageTag = FGameplayTag::RequestGameplayTag("Lyra.Ability.Casting.Message");
}

void UCastTimeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	StartCasting();
}


void UCastTimeAbility::StartCasting()
{
	CastTime = GetSectionStartTime(EndCastTimeSectionName, 1.0f);
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