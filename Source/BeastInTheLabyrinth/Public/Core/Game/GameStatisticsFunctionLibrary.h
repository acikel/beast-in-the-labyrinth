// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStatistics.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameStatisticsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API UGameStatisticsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Game Statistics", BlueprintPure)
	static TArray<FPlayerStatistics> GetPlayerStats(FGameStatistics GameStatistics);

	UFUNCTION(BlueprintCallable, Category = "Game Statistics")
	static FGameStatistics PrepareReplication(FGameStatistics GameStatistics);
};
