// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatistics.h"
#include "Maze/Maze.h"
#include "GameStatistics.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct BEASTINTHELABYRINTH_API FGameStatistics
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UMaze* Maze;

	UPROPERTY()
	TMap<uint32, FPlayerStatistics> PlayerStatistics;

	UPROPERTY(BlueprintReadOnly)
	TArray<FPlayerStatistics> PlayerStatisticsArray;

	
};
