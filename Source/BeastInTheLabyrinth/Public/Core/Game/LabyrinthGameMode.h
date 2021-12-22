// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CreatureSystem.h"
#include "GameStatisticsActor.h"
#include "LabyrinthGameState.h"
#include "Core/Game/BeastGameMode.h"
#include "Core/Objective/Objective.h"
#include "Maze/MazeGenerator.h"
#include "LabyrinthGameMode.generated.h"


/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API ALabyrinthGameMode : public ABeastGameMode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<AObjective>> Objectives;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void PlayersReadyToExit();
	
	UFUNCTION(BlueprintCallable)
	void LoseGame();

	UFUNCTION(BlueprintNativeEvent)
	void OnExitIsReady();
	void OnExitIsReady_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnGameEnded(const bool bIsWon);
	void OnGameEnded_Implementation(const bool bIsWon);
	
	void GenerateObjectives();
	void FindMazeGenerator();
	//void InstantiateObjectivesActors();

	UFUNCTION()
	void OnObjectiveCompleted();
	
	UPROPERTY(BlueprintReadOnly)
	AMazeGenerator* MazeGenerator;

	UPROPERTY(BlueprintReadOnly)
	ACreatureSystem* CreatureSystem;

	UPROPERTY(BlueprintReadOnly)
	AGameStatisticsActor* GameStatisticsActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ACreatureSystem> CreatureSystemClass;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPreMazeGenerate(AMazeGenerator* Generator);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPostMazeGenerate(AMazeGenerator* Generator);

	UPROPERTY()
	bool ReadyToPlay = false;
	
private:
	const int32 GENERATE_NUM_OBJECTIVES = 3;

public:
	ACreatureSystem* GetCreatureSystem() const { return CreatureSystem; };
	AGameStatisticsActor* GetGameStatistics() const { return GameStatisticsActor; }
	AMazeGenerator* GetMazeGenerator() const { return MazeGenerator; }

	bool IsReady() { return ReadyToPlay; };
};
