// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/CreatureSystem.h"

// Sets default values
ACreatureSystem::ACreatureSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACreatureSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACreatureSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

