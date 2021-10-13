// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/MazeActor/TileActor.h"

#include "Core/Game/Maze/Tile.h"


// Sets default values
ATileActor::ATileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName(TEXT("SceneRoot")));
	
	WallEast = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallEast")));
	WallNorth = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallNorth")));
	WallSouth = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallSouth")));
	WallWest = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallWest")));

	WallEast->SetRelativeLocation(FVector(-100, 0, 0));
	WallNorth->SetRelativeLocation(FVector(0, -100, 0));
	WallSouth->SetRelativeLocation(FVector(0, 100, 0));
	WallWest->SetRelativeLocation(FVector(100, 0, 0));
	
	SetRootComponent(Root);
	WallEast->SetupAttachment(Root);
	WallNorth->SetupAttachment(Root);
	WallSouth->SetupAttachment(Root);
	WallWest->SetupAttachment(Root);
}

void ATileActor::Init(const UTile* Tile)
{
	uint8 tileValue = Tile->TileValue;
	TileValue = tileValue;

	const bool wallTop = tileValue & 1;
	const bool wallRight = tileValue & 2;
	const bool wallBottom = tileValue & 4;
	const bool wallLeft = tileValue & 8;

	if (wallTop)
		DisableWall(WallNorth);


	if (wallRight)
		DisableWall(WallWest);

	if (wallBottom)
		DisableWall(WallSouth);

	if (wallLeft)
		DisableWall(WallEast);
}

// Called when the game starts or when spawned
void ATileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileActor::DisableWall(UStaticMeshComponent* Wall)
{
	Wall->DestroyComponent();
}

