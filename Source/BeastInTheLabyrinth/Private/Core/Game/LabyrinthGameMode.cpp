// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/LabyrinthGameMode.h"

#include "EngineUtils.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"
#include "Core/Game/Maze/MazeGenerator.h"

struct FMazeActorSpawnInfo;

void ALabyrinthGameMode::BeginPlay()
{
	Super::BeginPlay();

	FindMazeGenerator();

	if(!MazeGenerator) return;
	
	GenerateObjectives();

	// Generate Maze
	MazeGenerator->GenerateRandomSeed();
	MazeGenerator->Generate();
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
		NewObjective->OnPostGeneration();

		for (const FMazeActorSpawnInfo &SpawnInfo : NewObjective->RequiredActors)
		{
			if(SpawnInfo.ActorClass)
			{
				UE_LOG(BeastGame, Log, TEXT("Instantiate actor of class: '%s'"), *SpawnInfo.ActorClass->GetName());
				MazeGenerator->AddActorToSpawn(SpawnInfo);
			}
			else
			{
				UE_LOG(BeastGame, Error, TEXT("Objective '%s' has no class set for required item"), *NewObjective->GetName());
			}
		}
		
		
		LabyrinthGameState->ChosenObjectives.Add(NewObjective);
		AvailableObjectives.RemoveAt(RandomIndex);
	}
}

void ALabyrinthGameMode::FindMazeGenerator()
{
	UE_LOG(BeastGame, Log, TEXT("=== Generate the labyrinth ==="));

	//TArray<AActor*> FoundActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);
	
	for (TActorIterator<AMazeGenerator> Generator(GetWorld()); Generator; ++Generator)
	{
		UE_LOG(BeastGame, Log, TEXT("Generate maze %s"), *Generator->GetName());
		MazeGenerator = *Generator;
	}
}

// void ALabyrinthGameMode::InstantiateObjectivesActors()
// {
// 	UE_LOG(BeastGame, Log, TEXT("=== Instantiate objectives actors ==="));
//
// 	ALabyrinthGameState* LabyrinthGameState = GetGameState<ALabyrinthGameState>();
// 	for (UObjective* Objective : LabyrinthGameState->ChosenObjectives)
// 	{
// 		Objective->OnRequiredActorsSpawned();
// 	}
// }

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
