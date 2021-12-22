// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/GameStatisticsFunctionLibrary.h"

TArray<FPlayerStatistics> UGameStatisticsFunctionLibrary::GetPlayerStats(FGameStatistics GameStatistics)
{
	TArray<FPlayerStatistics> PlayerStats;
	GameStatistics.PlayerStatistics.GenerateValueArray(PlayerStats);

	return PlayerStats;
}

FGameStatistics UGameStatisticsFunctionLibrary::PrepareReplication(FGameStatistics GameStatistics)
{
	GameStatistics.PlayerStatistics.GenerateValueArray(GameStatistics.PlayerStatisticsArray);
	if (GameStatistics.Maze)
	{
		const int32 MazeTileCount = GameStatistics.Maze->Width * GameStatistics.Maze->Height;
		for (int32 i = 0; i < GameStatistics.PlayerStatisticsArray.Num(); ++i)
		{
			GameStatistics.PlayerStatisticsArray[i].MazeCoverage = static_cast<float>(GameStatistics.PlayerStatisticsArray[i].CoveredTiles.Num()) / static_cast<float>(MazeTileCount);
		}
	}
	
	return GameStatistics;
}
