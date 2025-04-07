// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPCController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ANPCController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
protected:
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
};
