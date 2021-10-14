// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Game/Maze/Isle.h"
#include "Core/Game/MazeActor/TileActor.h"
#include "MazeGenerator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API AMazeGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	AMazeGenerator();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntPoint MazeSize = FIntPoint(20, 20);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UIsle>> IsleCatalogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UIsle>> RequiredIsles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ATileActor>> TileActors;

	UPROPERTY(ReplicatedUsing=OnRep_Seed, EditAnywhere, BlueprintReadWrite)
	int32 Seed;

	UFUNCTION(BlueprintCallable, CallInEditor)
	void Generate();

	UFUNCTION(BlueprintCallable)
	int32 GenerateRandomSeed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSeedReplicated();
	

private:
	UPROPERTY()
	class UMaze* Maze;

	UFUNCTION()
	void OnRep_Seed();
	
	void SpawnMaze();

	void SpawnIsles();
	void SpawnTiles();
	void SpawnActors();

	void DebugPrintMaze();
};
