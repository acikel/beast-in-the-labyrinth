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
	FURL Url;
	Url.Map = LevelName;
	Url.AddOption(TEXT("game=/Game/BeastInTheLabyrinth/Core/Game/BP_LabyrinthGameMode.BP_LabyrinthGameMode_C"));
	Url.AddOption(TEXT("listen"));
	
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Please start game as 'Standalone Game'"));    
	}
	
	GetWorld()->ServerTravel(Url.ToString());
}
