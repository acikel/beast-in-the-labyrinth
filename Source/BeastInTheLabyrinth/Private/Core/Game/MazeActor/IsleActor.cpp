// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/MazeActor/IsleActor.h"

#include "Core/Game/Maze/Isle.h"


// Sets default values
AIsleActor::AIsleActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AIsleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIsleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

