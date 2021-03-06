// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/LabyrinthGameMode.h"

#include "EngineUtils.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"
#include "Core/Game/BeastGameInstance.h"
#include "Core/Game/Maze/MazeGenerator.h"
#include "Core/Game/Maze/MazeActorSpawnInfo.h"

struct FMazeActorSpawnInfo;


void ALabyrinthGameMode::BeginPlay()
{
	Super::BeginPlay();

	UBeastGameInstance* GameInstance = Cast<UBeastGameInstance>(GetGameInstance());
	GameStatisticsActor = Cast<AGameStatisticsActor>(GetWorld()->SpawnActor(AGameStatisticsActor::StaticClass()));

	
	CreatureSystem = Cast<ACreatureSystem>(GetWorld()->SpawnActor(CreatureSystemClass));

	FindMazeGenerator();

	if(!MazeGenerator) return;
	
	GenerateObjectives();


	OnPreMazeGenerate(MazeGenerator);

	// Generate Maze
	MazeGenerator->GenerateRandomSeed();
	MazeGenerator->Generate();

	OnPostMazeGenerate(MazeGenerator);
	
	GameInstance->GameStatistics.Maze = MazeGenerator->GetMaze();
	ReadyToPlay = true;
}

void ALabyrinthGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ALabyrinthGameState* LabyrinthGameState = GetGameState<ALabyrinthGameState>();
	for (AObjective* Objective : LabyrinthGameState->ChosenObjectives)
	{
		if (Objective)
		{
			Objective->OnCompleted.RemoveAll(this);
		}
	}
}

void ALabyrinthGameMode::PlayersReadyToExit()
{
	OnGameEnded(true);
}

void ALabyrinthGameMode::LoseGame()
{
	OnGameEnded(false);
}

void ALabyrinthGameMode::GenerateObjectives()
{
	TArray<UClass*> AvailableObjectives = TArray<UClass*>(Objectives);
	ALabyrinthGameState* LabyrinthGameState = GetGameState<ALabyrinthGameState>();
	
	LabyrinthGameState->ChosenObjectives.Empty();
	
	while (AvailableObjectives.Num() > 0 && LabyrinthGameState->ChosenObjectives.Num() < GENERATE_NUM_OBJECTIVES)
	{
		const int32 RandomIndex = FMath::RandRange(0, AvailableObjectives.Num() - 1);

		if(!AvailableObjectives[RandomIndex])
		{
			UE_LOG(BeastGame, Error, TEXT("There is a null pointer configured within the objectives array in LabyrinthGameMode"));
			break;
		}
		
		//AObjective* NewObjective = NewObject<AObjective>(this, AvailableObjectives[RandomIndex]);
		AObjective* NewObjective = GetWorld()->SpawnActor<AObjective>(AvailableObjectives[RandomIndex]);
		
		NewObjective->OnCompleted.AddDynamic(this, &ALabyrinthGameMode::OnObjectiveCompleted);
		NewObjective->OnPostGeneration();

		for (UMazeActorSpawnInfo* SpawnInfo : NewObjective->RequiredActors)
		{
			if(SpawnInfo->ActorClass)
			{
				MazeGenerator->AddActorToSpawn(SpawnInfo);
			}
			else
			{
				UE_LOG(BeastGame, Error, TEXT("Objective '%s' has no class set for required item"), *NewObjective->GetName());
			}
		}

		for (TSubclassOf<UIsle> Isle : NewObjective->RequiredIsle)
		{
			if(Isle != nullptr)
			{
				MazeGenerator->RequiredIsles.Add(Isle);
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
	for (const AObjective* Objective : LabyrinthGameState->ChosenObjectives)
	{
		// If any objective isn't completed - The game goes on
		if(!Objective->IsCompleted()) { return; }
	}

	UE_LOG(BeastGame, Log, TEXT("All objectives are solved"));

	OnExitIsReady();
}

void ALabyrinthGameMode::OnExitIsReady_Implementation()
{
	UE_LOG(BeastGame, Log, TEXT("OnExitIsReady"));
}

void ALabyrinthGameMode::OnGameEnded_Implementation(const bool bIsWon)
{
	UE_LOG(BeastGame, Log, TEXT("Game has ended! Is won: %d"), bIsWon);
}


