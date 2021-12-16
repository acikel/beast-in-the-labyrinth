// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Maze.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BEASTINTHELABYRINTH_API UMaze : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 Width;

	UPROPERTY(BlueprintReadOnly)
	int32 Height;

	UPROPERTY()
	int32 Seed;

	UPROPERTY()
	int32 IsleCount = 3;

	UPROPERTY()
	TArray<TSubclassOf<class UIsle>> IsleCatalogue;
	
	TArray<TArray<class UTile*>> Tiles;

	UPROPERTY()
	TArray<TSubclassOf<class UIsle>> RequiredIsles;
	
	UPROPERTY()
	TArray<class UIsle*> Isles;

	void Generate(int32 MazeWidth = 30, int32 MazeHeight = 30);

private:
	TArray<class UTile*> OpenTiles;

	TMap<class UIsle*, TArray<class UTile*>> IsleAdjacentTiles;
	
	FRandomStream Random;

	void PlaceIsles();
	void PlaceTiles();
	void ConnectIsles();

	bool DoesIsleFit(class UIsle* Isle, int32 x, int32 y);
	void PlaceIsle(class UIsle* Isle, int32 x, int32 y);

	void RemoveOpenTile(class UTile* Tile);

	void AddAdjacentTile(class UIsle* Isle, class UTile* Tile);
	void RemoveAdjacentTile(class UIsle* Isle, class UTile* Tile);
};
