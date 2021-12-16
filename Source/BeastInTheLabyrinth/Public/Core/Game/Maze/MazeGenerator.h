// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maze.h"

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
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	FIntPoint MazeSize = FIntPoint(20, 20);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int32 TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UIsle>> IsleCatalogue;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UIsle>> RequiredIsles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<ATileActor>> TileActors;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<class UMazeActorSpawnInfo*> Actors;

	UPROPERTY(ReplicatedUsing=OnRep_Seed, EditAnywhere, BlueprintReadWrite)
	int32 Seed;

	UFUNCTION(BlueprintCallable, CallInEditor)
	void Generate();

	UFUNCTION(BlueprintCallable)
	int32 GenerateRandomSeed();

	UFUNCTION(BlueprintCallable)
	void AddActorToSpawn(UMazeActorSpawnInfo* ActorSpawnInfo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	TArray<FTransform> GetPlayerStartPoints() { return PlayerSpawnPoints; }

	UFUNCTION(BlueprintCallable)
	FTransform GetCreatureStartPoint() { return CreatureSpawnPoint; }

	UFUNCTION()
	class UMaze* GetMaze() { return Maze; }

	int32 GetTileIdAtLocation(FVector Location);
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSeedReplicated();
	

private:
	UPROPERTY()
	class UMaze* Maze;

	UFUNCTION()
	void OnRep_Seed();

	FRandomStream Random;
	
	void SpawnMaze();

	void SpawnIsles();
	void SpawnTiles();
	void SpawnActors();
	void GenerateStartingPoints();

	TArray<int32> GetTileIndexesToSpawnOn(int32 NumberOfActors, bool RequiresWall);

	void PlaceActorAbsolute(UMazeActorSpawnInfo* ActorSpawnInfo);
	void PlaceActorFixedAmount(UMazeActorSpawnInfo* ActorSpawnInfo);
	void PlaceActorDensity(UMazeActorSpawnInfo* ActorSpawnInfo);

	void PlaceActorOnTile(UMazeActorSpawnInfo* ActorSpawnInfo, int32 TileIndex);

	class UTile* GetTileAtIndex(int32 Index);
	FVector GetTileLocation(const class UTile* Tile);
	FVector GetTileLocation(int32 X, int32 Y);
	
	
	FVector MazeOrigin;
	TArray<FTransform> PlayerSpawnPoints;
	FTransform CreatureSpawnPoint;

	void DebugPrintMaze();
};

