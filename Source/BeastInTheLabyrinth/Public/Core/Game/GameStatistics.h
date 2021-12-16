// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatistics.h"
#include "Maze/Maze.h"
#include "UObject/NoExportTypes.h"
#include "GameStatistics.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API UGameStatistics : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UMaze* Maze;

	UPROPERTY()
	TMap<uint32, FPlayerStatistics> PlayerStatistics;

	UFUNCTION(BlueprintCallable)
	TArray<FPlayerStatistics> GetPlayerStatistics() const;

	void AddStatisticForPlayer(uint32 PlayerId, const FString PlayerName);
};
