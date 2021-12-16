// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/GameStatistics.h"

TArray<FPlayerStatistics> UGameStatistics::GetPlayerStatistics() const
{
	TArray<FPlayerStatistics> PlayerStatisticArray;
	PlayerStatistics.GenerateValueArray(PlayerStatisticArray);

	return PlayerStatisticArray;
}

void UGameStatistics::AddStatisticForPlayer(uint32 PlayerId, const FString PlayerName)
{
	FString Name = PlayerName;
	FPlayerStatistics Statistics = PlayerStatistics.Add(PlayerId, FPlayerStatistics(PlayerId, Name));
}
