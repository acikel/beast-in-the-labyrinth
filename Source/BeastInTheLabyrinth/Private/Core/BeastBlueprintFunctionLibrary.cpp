// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BeastBlueprintFunctionLibrary.h"

#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"
#include "Core/Game/LabyrinthGameMode.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"


void UBeastBlueprintFunctionLibrary::GetSessionName(const FBlueprintSessionResult SessionResult, FString& SessionName)
{
	if(!SessionResult.OnlineResult.Session.SessionSettings.Get(FName("SESSION_NAME"), SessionName))
	{
		UE_LOG(BeastGame, Warning, TEXT("Could not get session name"));
	}
}

FString UBeastBlueprintFunctionLibrary::ToString(const ENetworkFailure::Type FailureType)
{
	return ENetworkFailure::ToString(FailureType);
}

ACreatureSystem* UBeastBlueprintFunctionLibrary::GetCreatureSystem(UObject* Target)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(Target);
	ALabyrinthGameMode* LabyrinthGameMode = Cast<ALabyrinthGameMode>(GameMode);

	if (LabyrinthGameMode)
	{
		return LabyrinthGameMode->GetCreatureSystem();
	}
	
	return nullptr;
}

AGameStatisticsActor* UBeastBlueprintFunctionLibrary::GetGameStatisticsActor(UObject* Target)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(Target);
	ALabyrinthGameMode* LabyrinthGameMode = Cast<ALabyrinthGameMode>(GameMode);

	if (LabyrinthGameMode)
	{
		return LabyrinthGameMode->GetGameStatistics();
	}
	
	return nullptr;
}
