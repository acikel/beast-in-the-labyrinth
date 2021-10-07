// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/BeastPlayerState.h"

#include "Net/UnrealNetwork.h"


void ABeastPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABeastPlayerState, DisplayName);
	DOREPLIFETIME(ABeastPlayerState, bIsGroupLeader);
}

void ABeastPlayerState::SetNickname_Implementation(const FString& PlayerName)
{
	DisplayName = PlayerName;
}

void ABeastPlayerState::SetIsLeader(const bool bIsLeader)
{
	if(!HasAuthority()) { return; }

	this->bIsGroupLeader = bIsLeader;
}

FString ABeastPlayerState::GetNickname() const
{
	return DisplayName;
}


// FString ABeastPlayerState::GetPlayerNameCustom() const
// {
// 	return DisplayName;
// }
