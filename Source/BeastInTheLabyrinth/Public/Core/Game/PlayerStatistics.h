// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatistics.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BEASTINTHELABYRINTH_API FPlayerStatistics
{
	GENERATED_BODY()

public:
	FPlayerStatistics()
	{
	}

	FPlayerStatistics(uint32 NewPlayerId, FString& NewPlayerName)
	{
		PlayerId = NewPlayerId;
		PlayerName = NewPlayerName;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PlayerName = "";

	UPROPERTY(EditAnywhere)
	uint32 PlayerId = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeCloseToCreature = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DistanceTravelled = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 AmountOfDeaths = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 AmountOfRevivesGiven = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int32> CoveredTiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MazeCoverage = 0;
};
