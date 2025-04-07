#pragma once
#include "DungeonCrawler/DungeonCrawlerCharacter.h"
#include "ChangeHealthUIDisplayMessage.generated.h"

USTRUCT(BlueprintType)
struct FChangeHealthUIDisplayMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category= "GameplayMessage")
	FText HealthTextValue;

	UPROPERTY(BlueprintReadWrite, Category= "GameplayMessage")
	float HealthPercentage;

	UPROPERTY(BlueprintReadWrite, Category= "GameplayMessage")
	TWeakObjectPtr<const ADungeonCrawlerCharacter> ActorReporting;

	// Returns a debug string representation of this message
	DUNGEONCRAWLER_API FString ToString() const;
};