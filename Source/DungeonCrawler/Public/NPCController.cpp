// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCController.h"

#include "DungeonCrawler/DungeonCrawlerCharacter.h"
#include "Perception/AIPerceptionSystem.h"

void ANPCController::SetGenericTeamId(const FGenericTeamId& Other)
{
	Super::SetGenericTeamId(Other);
	UAIPerceptionSystem::GetCurrent(GetWorld())->UpdateListener(*GetAIPerceptionComponent());
}

ETeamAttitude::Type ANPCController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if (OtherPawn == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	const auto CharacterTeamAgent = Cast<IGenericTeamAgentInterface>(&Other);
	const IGenericTeamAgentInterface* ControllerTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (CharacterTeamAgent == nullptr && ControllerTeamAgent == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	FGenericTeamId OtherTeamID = FGenericTeamId();
	if (ControllerTeamAgent != nullptr)
	{
		OtherTeamID = ControllerTeamAgent->GetGenericTeamId();
	}
	else if (CharacterTeamAgent != nullptr)
	{
		OtherTeamID = CharacterTeamAgent->GetGenericTeamId();
	}

	const FGenericTeamId MyTeamID = FGenericTeamId();
	if (OtherTeamID == EFaction::NonCombatants)
	{
		return ETeamAttitude::Neutral;
	}

	if (OtherTeamID == MyTeamID)
	{
		return ETeamAttitude::Friendly;
	}

	return ETeamAttitude::Hostile;
}

