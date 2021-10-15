// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/Maze/MazeGenerator.h"

#include "Core/Game/Maze/Maze.h"
#include "Core/Game/Maze/Tile.h"
#include "Net/UnrealNetwork.h"

AMazeGenerator::AMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	FRandomStream Random;
	Random.GenerateNewSeed();

	Seed = Random.GetCurrentSeed();
	TileSize = 200;
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

int32 AMazeGenerator::GenerateRandomSeed()
{
	FRandomStream Random;
	Random.GenerateNewSeed();
	Seed = Random.GetCurrentSeed();

	return Seed;
}

void AMazeGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMazeGenerator, Seed);
}

void AMazeGenerator::OnRep_Seed()
{
	OnSeedReplicated();
}

void AMazeGenerator::SpawnMaze()
{
	const FVector Location = GetActorLocation();
	MazeOrigin = FVector(Location.X - (TileSize * (MazeSize.X * 0.5f)), Location.Y - (TileSize * (MazeSize.Y * 0.5f)), Location.Z);
	
	SpawnIsles();
	SpawnTiles();
	SpawnActors();
}

void AMazeGenerator::SpawnIsles()
{
	for (auto Isle : Maze->Isles)
	{
		FVector Location(
			MazeOrigin.X + Isle->Position.X * TileSize,
			MazeOrigin.Y + Isle->Position.Y * TileSize,
			MazeOrigin.Z);
		FRotator Rotator(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnParameters;
		
		GetWorld()->SpawnActor<AIsleActor>(Isle->IsleActor, Location, Rotator, SpawnParameters);
	}
}

void AMazeGenerator::SpawnTiles()
{
	FRandomStream Random(Seed);
	
	for (int32 y = 0; y < Maze->Tiles.Num(); ++y)
	{
		for (int32 x = 0; x < Maze->Tiles[y].Num(); ++x)
		{
			const UTile* Tile = Maze->Tiles[y][x];
			if (Tile)
			{
				FVector Location(
					MazeOrigin.X + (Tile->Position.X * TileSize + (TileSize * 0.5f)),
					MazeOrigin.Y + (Tile->Position.Y * TileSize + (TileSize * 0.5f)),
					MazeOrigin.Z);
				
				FRotator Rotator(0.0f, 0.0f, 0.0f);
				FActorSpawnParameters SpawnParameters;

				UTile* TileAbove = nullptr;
				UTile* TileLeft = nullptr;

				if (y - 1 >= 0)
					TileAbove = Maze->Tiles[y - 1][x];
				if (x - 1 >= 0)
					TileLeft = Maze->Tiles[y][x - 1];

				TileActorSpawnInfo SpawnInfo;
				uint8 tileValue = Tile->TileValue;
				SpawnInfo.WallTop = !(tileValue & 1);
				SpawnInfo.WallRight = !(tileValue & 2);
				SpawnInfo.WallBottom = !(tileValue & 4);
				SpawnInfo.WallLeft = !(tileValue & 8);
				
				if (TileAbove)
				{
					SpawnInfo.PillarNorthEast = false;
					SpawnInfo.PillarNorthWest = false;
					SpawnInfo.WallTop = false;
				}
				if (TileLeft)
				{
					SpawnInfo.PillarSouthEast = false;
					SpawnInfo.PillarNorthEast = false;
					SpawnInfo.WallLeft = false;
				}
			
				ATileActor* TileActor = GetWorld()->SpawnActor<ATileActor>(TileActors[Random.RandRange(0, TileActors.Num() - 1)], Location, Rotator, SpawnParameters);
				TileActor->Init(Tile, SpawnInfo);
			}
		}
	}
}

void AMazeGenerator::SpawnActors()
{
	if (HasAuthority())
	{
		// Only the server should spawn the Actors which will be replicated on their own
		
	}
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
