// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/Maze/Maze.h"
#include "Core/Game/Maze/Tile.h"
#include "Core/Game/Maze/Isle.h"

void UMaze::Generate(int32 MazeWidth, int32 MazeHeight)
{
	Width = MazeWidth;
	Height = MazeHeight;
	Random = FRandomStream(Seed);

	Tiles.SetNumZeroed(Height);
	for (int32 y = 0; y < Height; ++y)
	{
		Tiles[y].SetNumZeroed(Width);
		for (int32 x = 0; x < Width; ++x)
		{
			UTile* tile = NewObject<UTile>();
			tile->Init(this, x, y);
			Tiles[y][x] = tile;
		}
	}

	PlaceIsles();
	PlaceTiles();
	ConnectIsles();
}

void UMaze::PlaceIsles()
{
	TArray<UIsle*> RequiredIslesToPlace;
	TArray<UIsle*> IslesToPlace;
	
	for (auto requiredIsle : RequiredIsles)
	{
		RequiredIslesToPlace.Add(NewObject<UIsle>(this, requiredIsle));
	}
	
	while ((RequiredIsles.Num() + IslesToPlace.Num()) < IsleCount)
	{
		IslesToPlace.Add(NewObject<UIsle>(this, IsleCatalogue[Random.RandRange(0, IsleCatalogue.Num() - 1)]));
	}

	int nextIsleIndex = -1;
	UIsle* nextIsle = nullptr;
	bool isRequiredIsle = false;
	int placementTries = 0;

	while (IslesToPlace.Num() > 0)
	{
		if (!nextIsle)
		{
			if (RequiredIslesToPlace.Num() > 0)
			{
				nextIsleIndex = Random.RandRange(0, RequiredIslesToPlace.Num() - 1);
				nextIsle = RequiredIslesToPlace[nextIsleIndex];
				isRequiredIsle = true;
			}
			else
			{
				nextIsleIndex = Random.RandRange(0, IslesToPlace.Num() - 1);
				nextIsle = IslesToPlace[nextIsleIndex];
				isRequiredIsle = false;
			}
			placementTries = 0;
		}

		int x = Random.RandRange(1, Width - nextIsle->Size.X);
		int y = Random.RandRange(1, Height - nextIsle->Size.Y);

		if (DoesIsleFit(nextIsle, x, y))
		{
			PlaceIsle(nextIsle, x, y);

			if (isRequiredIsle)
				RequiredIslesToPlace.RemoveAt(nextIsleIndex);
			else
				IslesToPlace.RemoveAt(nextIsleIndex);
			
			nextIsle = nullptr;
		}

		++placementTries;
		if (placementTries > 80)
		{
			UE_LOG(LogTemp, Error, TEXT("Isle %s could not be placed in the labyrinth"), *(nextIsle->GetName()));
			nextIsle = nullptr;

			if (isRequiredIsle)
			{
				RequiredIslesToPlace.RemoveAt(nextIsleIndex);
				UE_LOG(LogTemp, Error, TEXT("REQUIRED Isle %s could not be placed in the labyrinth"), *(nextIsle->GetName()));
			}
			else
			{
				IslesToPlace.RemoveAt(nextIsleIndex);
			}
		}
	}
}

void UMaze::PlaceTiles()
{
	while (OpenTiles.Num() > 0)
	{
		UTile* tile = OpenTiles[Random.RandRange(0, OpenTiles.Num() - 1)];

		TArray<UTile*> neighbours = tile->GetOpenNeighbours();
		if (neighbours.Num() == 0)
		{
			RemoveOpenTile(tile);
			continue;
		}

		UTile* neighbour = neighbours[Random.RandRange(0, neighbours.Num() - 1)];
		tile->Connect(neighbour);

		OpenTiles.Add(neighbour);

		if (neighbours.Num() == 1)
		{
			RemoveOpenTile(tile);
		}
	}
}

void UMaze::ConnectIsles()
{
	for (int32 i = 0; i < Isles.Num(); ++i)
	{
		UIsle* startingIsle = Isles[i];
		if (IsleAdjacentTiles.Contains(startingIsle))
		{
			for (int32 j = i + 1; j < Isles.Num(); ++j)
			{
				UIsle* goalIsle = Isles[j];
				
				TArray<UTile*> adjacentTiles;
				for (auto tile : IsleAdjacentTiles[startingIsle])
				{
					if (tile->Origin == goalIsle)
						adjacentTiles.Add(tile);
				}
				
				for (int n = 0; n < 3; ++n)
				{
					if (adjacentTiles.Num() > 0)
					{
						UTile* tile = adjacentTiles[Random.RandRange(0, adjacentTiles.Num() - 1)];
						UTile* neighbour = tile->GetNeighbourFromIsle(startingIsle);

						tile->Connect(neighbour);
						adjacentTiles.Remove(tile);
						adjacentTiles.Remove(neighbour);

						RemoveAdjacentTile(startingIsle, tile);
						RemoveAdjacentTile(goalIsle, neighbour);
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

bool UMaze::DoesIsleFit(UIsle* Isle, int32 x, int32 y)
{
	int32 xMax = x + Isle->Size.X + 1;
	int32 yMax = y + Isle->Size.Y + 1;
	
	// if (Isle->DoorsLeft.Num() > 0)
	// 	x -= 1;

	// if (Isle->DoorsTop.Num() > 0)
	// 	y -= 1;

	//if (Isle->DoorsRight.Num() > 0)
	// xMax += 1;

	//if (Isle->DoorsBottom.Num() > 0)
	// yMax += 1;

	if (x < 0 || y < 0)
		return false;

	if (yMax >= Tiles.Num())
		return false;

	if (xMax >= Tiles[0].Num())
		return false;

	for (int32 posY = y; posY < yMax; ++posY)
	{
		for (int32 posX = x; posX < xMax; ++posX)
		{
			if (!Tiles[posY][posX])
				return false;
			if (Tiles[posY][posX]->TileValue != 0)
				return false;
		}
	}

	return true;
}

void UMaze::PlaceIsle(UIsle* Isle, int32 x, int32 y)
{
	Isles.Add(Isle);
	
	int xMax = x + Isle->Size.X;
	int yMax = y + Isle->Size.Y;

	for (int32 posY = y; posY < yMax; ++posY)
	{
		for (int32 posX = x; posX < xMax; ++posX)
		{
			Tiles[posY][posX] = nullptr;
		}
	}

	for (auto door : Isle->DoorsLeft)
	{
		int32 posX = x - 1;
		int32 posY = y + door;

		UTile* tile = Tiles[posY][posX];
		tile->SetWallRight(false);
		tile->Origin = Isle;
		OpenTiles.Add(tile);
	}
	
	for (auto door : Isle->DoorsRight)
	{
		int32 posX = xMax;
		int32 posY = y + door;

		UTile* tile = Tiles[posY][posX];
		tile->SetWallLeft(false);
		tile->Origin = Isle;
		OpenTiles.Add(tile);
	}

	for (auto door : Isle->DoorsTop)
	{
		int32 posX = x + door;
		int32 posY = y - 1;

		UTile* tile = Tiles[posY][posX];
		tile->SetWallBottom(false);
		tile->Origin = Isle;
		OpenTiles.Add(tile);
	}

	for (auto door : Isle->DoorsBottom)
	{
		int32 posX = x + door;
		int32 posY = yMax;

		UTile* tile = Tiles[posY][posX];
		tile->SetWallTop(false);
		tile->Origin = Isle;
		OpenTiles.Add(tile);
	}

	Isle->Position = FIntPoint(x, y);
}

void UMaze::RemoveOpenTile(UTile* Tile)
{
	Tile->Closed = true;
	OpenTiles.Remove(Tile);

	TArray<UIsle*> adjacentIsles = Tile->GetAdjacentIsles();
	if (adjacentIsles.Num() > 0)
	{
		for (auto adjacentIsle : adjacentIsles)
		{
			AddAdjacentTile(adjacentIsle, Tile);
		}
	}
}

void UMaze::AddAdjacentTile(UIsle* Isle, UTile* Tile)
{
	if (!IsleAdjacentTiles.Contains(Isle))
		IsleAdjacentTiles.Add(Isle, TArray<UTile*>());

	IsleAdjacentTiles[Isle].Add(Tile);
}

void UMaze::RemoveAdjacentTile(UIsle* Isle, UTile* Tile)
{
	if (!IsleAdjacentTiles.Contains(Isle))
		return;

	IsleAdjacentTiles[Isle].Remove(Tile);
}
