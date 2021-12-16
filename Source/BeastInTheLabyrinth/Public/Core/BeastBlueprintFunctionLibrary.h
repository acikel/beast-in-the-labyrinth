// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FindSessionsCallbackProxy.h"
#include "Game/CreatureSystem.h"
#include "Runtime/CoreUObject/Public/UObject/NoExportTypes.h"
#include "BeastBlueprintFunctionLibrary.generated.h"



/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API UBeastBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Online|Session")
	static void GetSessionName(FBlueprintSessionResult SessionResult, FString& SessionName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameInstance")
	static FString ToString(ENetworkFailure::Type FailureType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Creature", meta = (WorldContext = Target))
	static UPARAM(DisplayName="CreatureSystem") class ACreatureSystem* GetCreatureSystem(UObject* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Statistics", meta = (WorldContext = Target))
	static UPARAM(DisplayName="GameStatistics") class AGameStatisticsActor* GetGameStatisticsActor(UObject* Target);
};
