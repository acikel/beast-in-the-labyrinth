// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/LabyrinthGameState.h"

#include "Net/UnrealNetwork.h"

void ALabyrinthGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALabyrinthGameState, ChosenObjectives, COND_InitialOnly);
}
