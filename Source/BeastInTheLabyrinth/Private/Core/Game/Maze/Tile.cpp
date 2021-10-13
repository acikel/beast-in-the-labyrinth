// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/Maze/Tile.h"

#include "Core/Game/Maze/Maze.h"

void UTile::Init(UMaze* OwningMaze, int32 X, int32 Y)
{
	Maze = OwningMaze;
	Position = FIntPoint(X, Y);
}

TArray<UTile*> UTile::GetOpenNeighbours()
{
	return GetNeighbours(true);
}

TArray<UTile*> UTile::GetNeighbours(bool OpenTilesOnly)
{
	TArray<UTile*> neighbours;

	if (Position.X + 1 < Maze->Width)
	{
		UTile* tile = GetNeighbour(Position.X + 1, Position.Y, OpenTilesOnly);
		if (tile)
			neighbours.Add(tile);
	}

	if (Position.X - 1 >= 0)
	{
		UTile* tile = GetNeighbour(Position.X - 1, Position.Y, OpenTilesOnly);
		if (tile)
			neighbours.Add(tile);
	}

	if (Position.Y + 1 < Maze->Height)
	{
		UTile* tile = GetNeighbour( Position.X, Position.Y + 1, OpenTilesOnly);
		if (tile)
			neighbours.Add(tile);
	}

	if (Position.Y - 1 >= 0)
	{
		UTile* tile = GetNeighbour(Position.X, Position.Y - 1, OpenTilesOnly);
		if (tile)
			neighbours.Add(tile);
	}

	return neighbours;
}

TArray<UIsle*> UTile::GetAdjacentIsles()
{
	TArray<UIsle*> adjacentIsles;
	TArray<UTile*> neighbours = GetNeighbours();

	for (auto neighbour : neighbours)
	{
		if (neighbour->Origin != Origin)
		{
			if (!adjacentIsles.Contains(neighbour->Origin))
				adjacentIsles.Add(neighbour->Origin);
		}
	}

	return adjacentIsles;
}

UTile* UTile::GetNeighbourFromIsle(UIsle* Isle)
{
	TArray<UTile*> neighbours = GetNeighbours();
	for (auto neighbour : neighbours)
	{
		if (neighbour->Origin == Isle)
			return neighbour;
	}

	return nullptr;
}

void UTile::Connect(UTile* Tile)
{
	int x = Tile->Position.X - this->Position.X;
	int y = Tile->Position.Y - this->Position.Y;

	if (FMath::Abs(x) > 1 || FMath::Abs(y) > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Tiles were connected which were too far apart"));
	}

	if (FMath::Abs(x) > FMath::Abs(y))
	{
		if (x > 0)
		{
			SetWallRight(false);
			Tile->SetWallLeft(false);
		}
		else
		{
			SetWallLeft(false);
			Tile->SetWallRight(false);
		}
	}
	else
	{
		if (y > 0)
		{
			SetWallBottom(false);
			Tile->SetWallTop(false);
		}
		else
		{
			SetWallTop(false);
			Tile->SetWallBottom(false);
		}
	}

	if (!Tile->Origin)
		Tile->Origin = Origin;
}

FString UTile::GetTileString()
{
	switch (TileValue)
	{
		default:
		case 0: return TEXT(" ");
		case 1: return TEXT("╹");
		case 2: return TEXT("╺");
		case 3: return TEXT("┗");
		case 4: return TEXT("╻");
		case 5: return TEXT("┃");
		case 6: return TEXT("┏");
		case 7: return TEXT("┣");
		case 8: return TEXT("╸");
		case 9: return TEXT("┛");
		case 10: return TEXT("━");
		case 11: return TEXT("┻");
		case 12: return TEXT("┓");
		case 13: return TEXT("┫");
		case 14: return TEXT("┳");
		case 15: return TEXT("╋");
	}
}

void UTile::UpdateTile()
{
	int tileValue = 0;

	if (!WallTop)
		tileValue |= 1;

	if (!WallRight)
		tileValue |= 2;

	if (!WallBottom)
		tileValue |= 4;

	if (!WallLeft)
		tileValue |= 8;

	TileValue = tileValue;
}

UTile* UTile::GetNeighbour(int x, int y, bool OpenTileOnly)
{
	UTile* tile = Maze->Tiles[y][x];
	if (OpenTileOnly)
	{
		if (tile && tile->TileValue == 0 && tile->Closed == false)
		{
			return tile;
		}
	}
	else
	{
		return tile;
	}

	return nullptr;
}
