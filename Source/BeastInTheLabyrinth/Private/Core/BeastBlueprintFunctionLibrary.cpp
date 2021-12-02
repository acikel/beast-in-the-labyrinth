// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BeastBlueprintFunctionLibrary.h"

#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"


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
