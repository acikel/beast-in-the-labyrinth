// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/CreatureSystem.h"

// Sets default values
ACreatureSystem::ACreatureSystem()
{
	PrimaryActorTick.bCanEverTick = true;
 	PrimaryActorTick.bStartWithTickEnabled = true;

	DebugAggressionLevel.MaxSamples = 1000000;
	DebugAggressionLevel.MaxValue = 1;
	DebugAggressionLevel.MinValue = 0;
}

void ACreatureSystem::DebugSampleAggressionLevel()
{
	DebugAggressionLevel.AddSample(AggressionLevel);
}

// Called when the game starts or when spawned
void ACreatureSystem::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DebugAggressionTimer, this, &ACreatureSystem::DebugSampleAggressionLevel, 0.2f, true);
}

void ACreatureSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (AggressionLevel > AbsoluteAggressionLevel)
	{
		AbsoluteAggressionLevel += AbsoluteAggressionLevelIncrement * DeltaSeconds;
		AggressionLevel = FMath::InterpEaseInOut(AggressionLevel, AbsoluteAggressionLevel, DecreaseEasingAlpha, 2.f);
	}

	if (AggressionLevel < AbsoluteAggressionLevel)
	{
		AggressionLevel = AbsoluteAggressionLevel;
	}

	DecreaseEasingAlpha += DeltaSeconds / (CalmDownTime * 10);
	DecreaseEasingAlpha = FMath::Clamp(DecreaseEasingAlpha, 0.f, 1.f);
}



