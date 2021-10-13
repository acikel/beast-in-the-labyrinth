// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tile.generated.h"

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API UTile : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(class UMaze* OwningMaze, int32 X, int32 Y);
	
	UPROPERTY()
	class UIsle* Origin;

	UPROPERTY()
	uint8 TileValue = 0;

	UPROPERTY()
	bool Closed = false;

	void SetWallLeft(bool wallValue) { WallLeft = wallValue; UpdateTile(); }
	void SetWallTop(bool wallValue) { WallTop = wallValue; UpdateTile(); }
	void SetWallRight(bool wallValue) { WallRight = wallValue; UpdateTile(); }
	void SetWallBottom(bool wallValue) { WallBottom = wallValue; UpdateTile(); }

	TArray<UTile*> GetOpenNeighbours();
	TArray<UTile*> GetNeighbours(bool OpenTilesOnly = false);

	TArray<class UIsle*> GetAdjacentIsles();
	UTile* GetNeighbourFromIsle(UIsle* Isle);
	
	void Connect(UTile* Tile);

	UPROPERTY()
	FIntPoint Position;

	UFUNCTION()
	FString GetTileString();
	
private:
	
	UPROPERTY()
	bool WallLeft = true;
	
	UPROPERTY()
	bool WallTop = true;

	UPROPERTY()
	bool WallRight = true;

	UPROPERTY()
	bool WallBottom = true;

	UPROPERTY()
	class UMaze* Maze;

	void UpdateTile();
	UTile* GetNeighbour(int x, int y, bool OpenTileOnly = false);
};
