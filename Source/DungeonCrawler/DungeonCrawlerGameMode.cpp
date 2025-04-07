// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCrawlerGameMode.h"
#include "DungeonCrawlerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADungeonCrawlerGameMode::ADungeonCrawlerGameMode()
{
	// set default pawn class to our Blueprinted character
	//"/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/Classes/BP_Tank"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Gamemodes/BP_DungeonCrawlerPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
