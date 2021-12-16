// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/Maze/MazeGenerator.h"
#include "Core/Game/Maze/MazeActorSpawnInfo.h"

#include "Core/Game/Maze/Maze.h"
#include "Core/Game/Maze/Tile.h"
#include "Net/UnrealNetwork.h"

AMazeGenerator::AMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	Random.GenerateNewSeed();
	Seed = Random.GetCurrentSeed();
	TileSize = 200;
}

void AMazeGenerator::Generate()
{
	Random = FRandomStream(Seed);
	
	Maze = NewObject<UMaze>();
	Maze->Seed = Seed;
	
	Maze->IsleCatalogue = IsleCatalogue;
	Maze->RequiredIsles = RequiredIsles;

	Maze->Generate(MazeSize.X, MazeSize.Y);

	// DebugPrintMaze();
	SpawnMaze();
}

int32 AMazeGenerator::GenerateRandomSeed()
{
	Random.GenerateNewSeed();
	Seed = Random.GetCurrentSeed();

	return Seed;
}

void AMazeGenerator::AddActorToSpawn(UMazeActorSpawnInfo* ActorSpawnInfo)
{
	Actors.Add(ActorSpawnInfo);
}

void AMazeGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMazeGenerator, Seed);
	DOREPLIFETIME(AMazeGenerator, MazeSize);
	DOREPLIFETIME(AMazeGenerator, TileSize);
	DOREPLIFETIME(AMazeGenerator, Actors);
	DOREPLIFETIME(AMazeGenerator, RequiredIsles);
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
	GenerateStartingPoints();
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
	for (int32 y = 0; y < Maze->Tiles.Num(); ++y)
	{
		for (int32 x = 0; x < Maze->Tiles[y].Num(); ++x)
		{
			const UTile* Tile = Maze->Tiles[y][x];
			if (Tile)
			{
				FVector Location = GetTileLocation(Tile);
				
				FRotator Rotator(0.0f, 0.0f, 0.0f);
				FActorSpawnParameters SpawnParameters;

				UTile* TileAbove = nullptr;
				UTile* TileLeft = nullptr;

				if (y - 1 >= 0)
					TileAbove = Maze->Tiles[y - 1][x];
				if (x - 1 >= 0)
					TileLeft = Maze->Tiles[y][x - 1];

				FTileActorSpawnInfo SpawnInfo;
				SpawnInfo.Generated = true;
				SpawnInfo.Id = y * Maze->Width + x;
				
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

		for (auto ActorSpawnInfo : Actors)
		{
			switch (ActorSpawnInfo->DistributionType)
			{
				case EActorDistributionType::DENSITY:
					PlaceActorDensity(ActorSpawnInfo);
					break;
				default:
				case EActorDistributionType::ABSOLUTE:
					PlaceActorAbsolute(ActorSpawnInfo);
					break;
				case EActorDistributionType::FIXED_AMOUNT:
					PlaceActorFixedAmount(ActorSpawnInfo);
					break;
			}
		}
	}
}

void AMazeGenerator::GenerateStartingPoints()
{
	PlayerSpawnPoints.Empty();
	
	FIntPoint MazeHalfSize = MazeSize / 2;
	FIntPoint MazeQuarterSize = MazeSize / 4;

	int spawnCenterX = Random.RandRange(0, MazeHalfSize.X - 1) + MazeHalfSize.X;
	int spawnCenterY = Random.RandRange(0, MazeHalfSize.Y - 1) + MazeHalfSize.Y;

	TArray<FIntPoint> PlayerSpawnTiles;
	
	
	for (int32 i = 0; i < 4; ++i)
	{
		FIntPoint playerTile;
		playerTile.X = spawnCenterX + Random.RandRange(0, MazeQuarterSize.X) - MazeQuarterSize.X;
		playerTile.Y = spawnCenterY + Random.RandRange(0, MazeQuarterSize.Y) - MazeQuarterSize.Y;

		if (PlayerSpawnTiles.Contains(playerTile))
		{
			--i;
			continue;
		}

		PlayerSpawnTiles.Add(playerTile);

		FTransform PlayerSpawnPoint;
		PlayerSpawnPoint.SetLocation(GetTileLocation(playerTile.X, playerTile.Y));
		PlayerSpawnPoint.SetRotation(FQuat::MakeFromEuler(FVector(0.0f, 0.0f, Random.FRandRange(0.0f, 359.0f))));

		PlayerSpawnPoints.Add(PlayerSpawnPoint);
	}

	ETileWall Wall;
	
	if (FMath::Abs(spawnCenterX) > FMath::Abs(spawnCenterY))
	{
		if (spawnCenterX > MazeHalfSize.X)
			Wall = Left;
		else
			Wall = Right;
	}
	else
	{
		if (spawnCenterY > MazeHalfSize.Y)
			Wall = Top;
		else
			Wall = Bottom;
	}
	
	FIntPoint CreatureTile;

	switch (Wall)
	{
		case Top:
			CreatureTile.X = Random.RandRange(0, MazeSize.X - 1);
			CreatureTile.Y = Random.RandRange(0, 2);
			break;
		case Bottom:
			CreatureTile.X = Random.RandRange(0, MazeSize.X - 1);
			CreatureTile.Y = Random.RandRange(MazeSize.Y - 3, MazeSize.Y - 1);
			break;
		case Left:
			CreatureTile.X = Random.RandRange(0, 2);
			CreatureTile.Y = Random.RandRange(0, MazeSize.Y - 1);;
			break;
		case Right:
			CreatureTile.X = Random.RandRange(MazeSize.X - 3, MazeSize.X - 1);
			CreatureTile.Y = Random.RandRange(0, MazeSize.Y - 1);;
			break;
	}

	CreatureSpawnPoint.SetLocation(GetTileLocation(CreatureTile.X, CreatureTile.Y));
	CreatureSpawnPoint.SetRotation(FRotator(0.0f, 0.0f, Random.FRandRange(0.0f, 359.0f)).Quaternion());
}

TArray<int32> AMazeGenerator::GetTileIndexesToSpawnOn(int32 NumberOfActors, bool RequiresWall)
{
	int32 tileCount = MazeSize.X * MazeSize.Y;
	int32 count = FMath::Min(NumberOfActors, tileCount);

	TArray<int32> tileIndexes;
	int32 findTileTries = 0;
	int32 lastNum = 0;
	
	while (tileIndexes.Num() < count)
	{
		int32 index = Random.RandRange(0, tileCount - 1);
		if (!tileIndexes.Contains(index))
		{
			if (RequiresWall)
			{
				UTile* tile = GetTileAtIndex(index);
				if (!tile)
					continue;
					
				if (tile->HasWalls())
					tileIndexes.Add(index);
				else
					--count;
			}
			else
			{
				tileIndexes.Add(index);	
			}
		}
			

		if (lastNum == tileIndexes.Num())
		{
			++findTileTries;
			lastNum = tileIndexes.Num();
		}
		else
			findTileTries = 0;

		if (findTileTries > 100)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not find enough tiles to fit the DistributionValue"));
			break;
		}
	}

	return tileIndexes;
}

void AMazeGenerator::PlaceActorAbsolute(UMazeActorSpawnInfo* ActorSpawnInfo)
{
	AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorSpawnInfo->ActorClass, ActorSpawnInfo->AbsoluteTransform);
	ActorSpawnInfo->OnMazeActorSpawned.ExecuteIfBound(Actor);
}

void AMazeGenerator::PlaceActorFixedAmount(UMazeActorSpawnInfo* ActorSpawnInfo)
{
	int32 amount = static_cast<int32>(Random.FRandRange(ActorSpawnInfo->DistributionValue.GetLowerBoundValue(), ActorSpawnInfo->DistributionValue.GetUpperBoundValue()));
	TArray<int32> tileIndexes = GetTileIndexesToSpawnOn(amount, ActorSpawnInfo->PlaceWithinDistanceToWall);
	
	for (const int32 tileIndex : tileIndexes)
	{
		const int32 numActors = Random.RandRange(ActorSpawnInfo->SpawnAmountOnTile.GetLowerBound().GetValue(), ActorSpawnInfo->SpawnAmountOnTile.GetUpperBound().GetValue());
		for (int32 i = 0; i < numActors; ++i)
		{
			PlaceActorOnTile(ActorSpawnInfo, tileIndex);
		}
	}
}

void AMazeGenerator::PlaceActorDensity(UMazeActorSpawnInfo* ActorSpawnInfo)
{
	const float amount = Random.FRandRange(ActorSpawnInfo->DistributionValue.GetLowerBoundValue(), ActorSpawnInfo->DistributionValue.GetUpperBoundValue());
	const int32 tileAmount = FMath::Clamp(amount, 0.0f, 1.0f) * (MazeSize.X * MazeSize.Y);
	TArray<int32> tileIndexes = GetTileIndexesToSpawnOn(tileAmount, ActorSpawnInfo->PlaceWithinDistanceToWall);
	
	for (const int32 tileIndex : tileIndexes)
	{
		const int32 numActors = Random.RandRange(ActorSpawnInfo->SpawnAmountOnTile.GetLowerBound().GetValue(), ActorSpawnInfo->SpawnAmountOnTile.GetUpperBound().GetValue());
		for (int32 i = 0; i < numActors; ++i)
		{
			PlaceActorOnTile(ActorSpawnInfo, tileIndex);
		}
	}
}

void AMazeGenerator::PlaceActorOnTile(UMazeActorSpawnInfo* ActorSpawnInfo, int32 TileIndex)
{
	UTile* Tile = GetTileAtIndex(TileIndex);

	const float minX = (Tile && !Tile->HasWallLeft()) ? -TileSize * 0.5f : -TileSize * 0.4f;
	const float maxX = (Tile && !Tile->HasWallRight()) ? TileSize * 0.5f : TileSize * 0.4f;
	const float minY = (Tile && !Tile->HasWallTop()) ? -TileSize * 0.5f : -TileSize * 0.4f;
	const float maxY = (Tile && !Tile->HasWallBottom()) ? TileSize * 0.5f : TileSize * 0.4f;

	FVector Location;
	if (Tile)
		Location = GetTileLocation(Tile);
	else
		Location = GetTileLocation(TileIndex / MazeSize.X, TileIndex % MazeSize.X);

	FRotator Rotation = FRotator(0, Random.FRandRange(0, 359), 0);
	
	if (ActorSpawnInfo->PlaceWithinDistanceToWall)
	{
		if (!Tile)
		{
			UE_LOG(LogTemp, Error, TEXT("Tried to place an actor near a wall on an Isle-Tile"))
			return;
		}
		
		TArray<ETileWall> Walls = Tile->GetWalls();
		ETileWall wall = Walls[Random.RandRange(0, Walls.Num() - 1)];
		
		float distanceToWall = Random.FRandRange(ActorSpawnInfo->DistanceToWall.GetLowerBoundValue(), ActorSpawnInfo->DistanceToWall.GetUpperBoundValue());
		

		switch (wall)
		{
			default:
			case Top:
				Location.X += Random.FRandRange(minX, maxX);
				Location.Y += -(TileSize * 0.4f) + distanceToWall;
				break;
			case Bottom:
				Location.X += Random.FRandRange(minX, maxX);
				Location.Y += (TileSize * 0.4f) - distanceToWall;
				break;
			case Right:
				Location.X += (TileSize * 0.4f) - distanceToWall;
				Location.Y += Random.FRandRange(minY, maxY);
				break;
			case Left:
				Location.X += -(TileSize * 0.4f) + distanceToWall;
				Location.Y += Random.FRandRange(minY, maxY);
				break;
		}

		Rotation = UTile::GetWallRelativeRotation(wall);
		Rotation += FRotator(0.0f, Random.FRandRange(ActorSpawnInfo->RelativeZRotationToWall.GetLowerBoundValue(), ActorSpawnInfo->RelativeZRotationToWall.GetUpperBoundValue()), 0.0f);
	}
	else
	{
		Location.X += Random.FRandRange(minX, maxX);
		Location.Y += Random.FRandRange(minY, maxY);
	}

	
	AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorSpawnInfo->ActorClass, Location, Rotation);
	ActorSpawnInfo->OnMazeActorSpawned.ExecuteIfBound(Actor);
}

UTile* AMazeGenerator::GetTileAtIndex(int32 Index)
{
	if (Maze && Maze->Tiles.Num() > 0)
	{
		return Maze->Tiles[Index / MazeSize.X][Index % MazeSize.X];
	}
	return nullptr;
}

FVector AMazeGenerator::GetTileLocation(const UTile* Tile)
{
	return FVector(
	MazeOrigin.X + (Tile->Position.X * TileSize + (TileSize * 0.5f)),
	MazeOrigin.Y + (Tile->Position.Y * TileSize + (TileSize * 0.5f)),
	MazeOrigin.Z
	);
}

FVector AMazeGenerator::GetTileLocation(int32 X, int32 Y)
{
	return FVector(
	MazeOrigin.X + (X * TileSize + (TileSize * 0.5f)),
	MazeOrigin.Y + (Y * TileSize + (TileSize * 0.5f)),
	MazeOrigin.Z
	);
}

int32 AMazeGenerator::GetTileIdAtLocation(FVector Location)
{
	FVector RelativeLocation = Location - MazeOrigin;
	int32 y = RelativeLocation.Y / TileSize;
	int32 x = RelativeLocation.X / TileSize;

	return y * MazeSize.X + x;
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
