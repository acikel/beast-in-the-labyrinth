// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/MazeActor/TileActor.h"

#include "Core/Game/Maze/Tile.h"
#include "Core/Game/MazeActor/TileActorSpawnInfo.h"


// Sets default values
ATileActor::ATileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(FName(TEXT("SceneRoot")));
	
	WallEast = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallEast")));
	WallNorth = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallNorth")));
	WallSouth = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallSouth")));
	WallWest = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("WallWest")));

	PillarNorthWest = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("PillarNorthWest")));
	PillarNorthEast = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("PillarNorthEast")));
	PillarSouthWest = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("PillarSouthWest")));
	PillarSouthEast = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("PillarSouthEast")));
	
	WallNorth->SetRelativeLocation(FVector(0, -100, 0));
	WallSouth->SetRelativeLocation(FVector(0, 100, 0));
	WallEast->SetRelativeLocation(FVector(-100, 0, 0));
	WallWest->SetRelativeLocation(FVector(100, 0, 0));
	
	PillarNorthWest->SetRelativeLocation(FVector(100, -100, 0));
	PillarNorthEast->SetRelativeLocation(FVector(-100, -100, 0));
	PillarSouthWest->SetRelativeLocation(FVector(100, 100, 0));
	PillarSouthEast->SetRelativeLocation(FVector(-100, 100, 0));
	
	SetRootComponent(Root);
	WallEast->SetupAttachment(Root);
	WallNorth->SetupAttachment(Root);
	WallSouth->SetupAttachment(Root);
	WallWest->SetupAttachment(Root);
	PillarNorthWest->SetupAttachment(Root);
	PillarNorthEast->SetupAttachment(Root);
	PillarSouthWest->SetupAttachment(Root);
	PillarSouthEast->SetupAttachment(Root);
}

void ATileActor::Init(const UTile* Tile, const FTileActorSpawnInfo NewSpawnInfo)
{
	TileValue = Tile->TileValue;
	Id = SpawnInfo.Id;
	SpawnInfo = NewSpawnInfo;
	Init();
}

// Called when the game starts or when spawned
void ATileActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATileActor::PostLoad()
{
	Super::PostLoad();
	
	if (SpawnInfo.IsValid())
	{
		Init();
	}
}

// Called every frame
void ATileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileActor::DisableMeshComponent(UStaticMeshComponent* MeshComponent)
{
	MeshComponent->DestroyComponent();
}

void ATileActor::Init()
{
	if (!SpawnInfo.WallTop && WallNorth)
		DisableMeshComponent(WallNorth);
	
	if (!SpawnInfo.WallRight && WallWest)
		DisableMeshComponent(WallWest);

	if (!SpawnInfo.WallBottom && WallSouth)
		DisableMeshComponent(WallSouth);

	if (!SpawnInfo.WallLeft && WallEast)
		DisableMeshComponent(WallEast);

	if (!SpawnInfo.PillarNorthWest && PillarNorthWest)
		DisableMeshComponent(PillarNorthWest);

	if (!SpawnInfo.PillarNorthEast && PillarNorthEast)
		DisableMeshComponent(PillarNorthEast);

	if (!SpawnInfo.PillarSouthWest && PillarSouthWest)
		DisableMeshComponent(PillarSouthWest);

	if (!SpawnInfo.PillarSouthEast && PillarSouthEast)
		DisableMeshComponent(PillarSouthEast);
}

