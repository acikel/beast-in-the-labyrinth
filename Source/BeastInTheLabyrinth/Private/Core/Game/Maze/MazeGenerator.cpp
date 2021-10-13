// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/Maze/MazeGenerator.h"

#include "Core/Game/Maze/Maze.h"
#include "Core/Game/Maze/Tile.h"

AMazeGenerator::AMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	FRandomStream Random;
	Random.GenerateNewSeed();

	Seed = Random.GetCurrentSeed();
}

void AMazeGenerator::Generate()
{
	Maze = NewObject<UMaze>();
	Maze->Seed = Seed;
	
	Maze->IsleCatalogue = IsleCatalogue;
	Maze->RequiredIsles = RequiredIsles;

	Maze->Generate(MazeSize.X, MazeSize.Y);

	DebugPrintMaze();
	SpawnMaze();
}

void AMazeGenerator::SpawnMaze()
{
	SpawnIsles();
	SpawnTiles();
	SpawnActors();
}

void AMazeGenerator::SpawnIsles()
{
	for (auto Isle : Maze->Isles)
	{
		FVector Location(Isle->Position.X * 200.0f, Isle->Position.Y * 200.0f, 0.0f);
		FRotator Rotator(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParameters;
		
		GetWorld()->SpawnActor<AIsleActor>(Isle->IsleActor, Location, Rotator, SpawnParameters);
	}
}

void AMazeGenerator::SpawnTiles()
{
	for (int32 y = 0; y < Maze->Tiles.Num(); ++y)
	{
		for (int32 x = 0; x < Maze->Tiles[y].Num(); ++x)
		{
			const UTile* Tile = Maze->Tiles[y][x];
			if (Tile)
			{
				FVector Location(Tile->Position.X * 200.0f + 100.0f, Tile->Position.Y * 200.0f + 100.0f, 0.0f);
				FRotator Rotator(0.0f, 0.0f, 0.0f);
				FActorSpawnParameters SpawnParameters;
			
				ATileActor* TileActor = GetWorld()->SpawnActor<ATileActor>(TileActors[0], Location, Rotator, SpawnParameters);
				TileActor->Init(Tile);
			}
		}
	}
}

void AMazeGenerator::SpawnActors()
{
}

void AMazeGenerator::DebugPrintMaze()
{
	FString mazeText;
	
	for (int32 y = 0; y < Maze->Tiles.Num(); ++y)
	{
		for (int32 x = 0; x < Maze->Tiles[y].Num(); ++x)
		{
			UTile* Tile = Maze->Tiles[y][x];
			if (Tile)
			{
				mazeText.Append(Tile->GetTileString());
			}
			else
			{
				mazeText.Append(TEXT(" "));
			}
		}
		mazeText.Append("\r\n");
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *mazeText);
}
