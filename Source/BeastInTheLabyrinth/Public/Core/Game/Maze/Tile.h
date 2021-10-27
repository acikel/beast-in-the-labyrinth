// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tile.generated.h"

UENUM()
enum ETileWall
{
	Top = 1,
	Right = 2,
	Bottom = 4,
	Left = 8
};

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

	FORCEINLINE bool HasWalls() const { return TileValue < 15; }
	FORCEINLINE bool HasWall(ETileWall Wall) const { return !(TileValue & Wall); }
	bool HasWallTop() const { return !(TileValue & 1); }
	bool HasWallRight() const { return !(TileValue & 2); }
	bool HasWallBottom() const { return !(TileValue & 4); }
	bool HasWallLeft() const { return !(TileValue & 8); }

	TArray<ETileWall> GetWalls()
	{
		TArray<ETileWall> Walls;
		if (HasWall(ETileWall::Top))
			Walls.Add(ETileWall::Top);
		if (HasWall(ETileWall::Right))
			Walls.Add(ETileWall::Right);
		if (HasWall(ETileWall::Bottom))
			Walls.Add(ETileWall::Bottom);
		if (HasWall(ETileWall::Left))
			Walls.Add(ETileWall::Left);

		return Walls;
	}

	static FRotator GetWallRelativeRotation(ETileWall Wall)
	{
		switch (Wall)
		{
			case Top:
				return FRotator(0.0f, 0.0f, 0.0f);
			case Right:
				return FRotator(0.0f, 90.0f, 0.0f);
			case Bottom:
				return FRotator(0.0f, 180.0f, 0.0f);
			case Left:
				return FRotator(0.0f, 270.0f, 0.0f);
			default:
				return FRotator(0.0f, 0.0f, 0.0f);
		}
	};

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
