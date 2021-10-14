// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/LabyrinthGameMode.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"

void ALabyrinthGameMode::BeginPlay()
{
	Super::BeginPlay();

	GenerateObjectives();
	GenerateLabyrinth();
}

void ALabyrinthGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ALabyrinthGameState* LabyrinthGameState = GetGameState<ALabyrinthGameState>();
	for (UObjective* Objective : LabyrinthGameState->ChosenObjectives)
	{
		Objective->OnCompleted.RemoveAll(this);
	}
}

void ALabyrinthGameMode::OnGameEnded_Implementation(const bool bIsWon)
{
	UE_LOG(BeastGame, Log, TEXT("Game has ended! Is won: %d"), bIsWon);
}

void ALabyrinthGameMode::GenerateObjectives()
{
	TArray<UClass*> AvailableObjectives = TArray<UClass*>(Objectives);
	ALabyrinthGameState* LabyrinthGameState = GetGameState<ALabyrinthGameState>();
	
	LabyrinthGameState->ChosenObjectives.Empty();
	
	while (AvailableObjectives.Num() > 0 && LabyrinthGameState->ChosenObjectives.Num() < GENERATE_NUM_OBJECTIVES)
	{
		const int32 RandomIndex = FMath::RandRange(0, AvailableObjectives.Num() - 1);

		UObjective* NewObjective = NewObject<UObjective>(this, AvailableObjectives[RandomIndex]);

		NewObjective->OnCompleted.AddDynamic(this, &ALabyrinthGameMode::OnObjectiveCompleted);
		
		LabyrinthGameState->ChosenObjectives.Add(NewObjective);
		AvailableObjectives.RemoveAt(RandomIndex);
	}
}

void ALabyrinthGameMode::GenerateLabyrinth()
{
	UE_LOG(BeastGame, Log, TEXT("Generate the labyrinth"));
}

void ALabyrinthGameMode::OnObjectiveCompleted()
{
	ALabyrinthGameState* LabyrinthGameState = GetGameState<ALabyrinthGameState>();
	for (const UObjective* Objective : LabyrinthGameState->ChosenObjectives)
	{
		// If any objective isn't completed - The game goes on
		if(!Objective->IsCompleted()) { return; }
	}
	
	OnGameEnded(true);
}
