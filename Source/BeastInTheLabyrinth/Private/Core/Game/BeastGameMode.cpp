// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/BeastGameMode.h"

void ABeastGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	OnPlayerJoined.Broadcast(NewPlayer);
}

void ABeastGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	OnPlayerLeft.Broadcast(Exiting);
}

void ABeastGameMode::StartGame(const FString& LevelName)
{
	ServerTravel(LevelName, TEXT("BP_LabyrinthGameMode"));
}

void ABeastGameMode::ServerTravel(const FString& LevelName, const FString& GameMode)
{
	FURL Url;
	Url.Map = TEXT("/Game/BeastInTheLabyrinth/Map/") + LevelName;

	const FString GameModeArg = FString::Printf(TEXT("game=/Game/BeastInTheLabyrinth/Core/Game/%s.%s_C"), *GameMode, *GameMode);
	Url.AddOption(*GameModeArg);
	Url.AddOption(TEXT("listen"));
	
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Please start game as 'Standalone Game'"));    
	}
	
	GetWorld()->ServerTravel(Url.ToString());
}

