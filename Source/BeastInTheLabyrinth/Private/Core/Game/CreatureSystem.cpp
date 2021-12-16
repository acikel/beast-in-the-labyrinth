// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/CreatureSystem.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ACreatureSystem::ACreatureSystem()
{
	PrimaryActorTick.bCanEverTick = true;
 	PrimaryActorTick.bStartWithTickEnabled = true;

	DebugAggressionLevel.MaxSamples = 1000000;
	DebugAggressionLevel.MaxValue = 1;
	DebugAggressionLevel.MinValue = 0;

	bReplicates = true;
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

		if (Hunting && AggressionLevel < AggressionLevelRange.GetUpperBoundValue())
		{
			GetWorldTimerManager().ClearTimer(StopHuntingTimer);
			GetWorldTimerManager().SetTimer(StopHuntingTimer, this, &ACreatureSystem::StopHunting, 4, false);
		}
	}

	if (AggressionLevel < AbsoluteAggressionLevel)
	{
		AggressionLevel = AbsoluteAggressionLevel;
	}

	DecreaseEasingAlpha += DeltaSeconds / (CalmDownTime * 10);
	DecreaseEasingAlpha = FMath::Clamp(DecreaseEasingAlpha, 0.f, 1.f);
}

void ACreatureSystem::StopHunting()
{
	Hunting = false;
	OnCreatureCalmedDown.Broadcast();
}

void ACreatureSystem::IncreaseAggressionLevel(float increase)
{
	ensureMsgf(increase >= 0, TEXT("The 'increase' value has to be larger than or equal 0"));

	if (!HasAuthority())
	{
		ServerIncreaseAggressionLevel(increase);
		return;
	}
	
	AggressionLevel += increase;
	if (!AggressionLevelRange.Contains(AggressionLevel))
	{
		AggressionLevel = AggressionLevelRange.GetUpperBoundValue();
	}

	if (!Hunting && AggressionLevel == AggressionLevelRange.GetUpperBoundValue())
	{
		Hunting = true;
		OnCreatureAggressionLevelReached.Broadcast();
	}

	DecreaseEasingAlpha = 0;
}

void ACreatureSystem::DecreaseAggressionLevel(float decrease)
{
	ensureMsgf(decrease >= 0, TEXT("The 'decrease' value has to be larger than or equal 0"));
	
	if (!HasAuthority())
	{
		ServerDecreaseAggressionLevel(decrease);
		return;
	}
	
	AggressionLevel -= decrease;
	if (!AggressionLevelRange.Contains(AggressionLevel))
	{
		AggressionLevel = AggressionLevelRange.GetLowerBoundValue();
	}

	if (Hunting && AggressionLevel < AggressionLevelRange.GetUpperBoundValue())
	{
		Hunting = false;
		OnCreatureCalmedDown.Broadcast();
	}
}

void ACreatureSystem::DecreaseAbsoluteAggressionLevel(float decrease)
{
	ensureMsgf(decrease >= 0, TEXT("The 'decrease' value has to be larger than or equal 0"));
	
	if (!HasAuthority())
	{
		ServerDecreaseAbsoluteAggressionLevel(decrease);
		return;
	}
	
	AbsoluteAggressionLevel -= decrease;
	if (!AggressionLevelRange.Contains(AbsoluteAggressionLevel))
	{
		AbsoluteAggressionLevel = AggressionLevelRange.GetLowerBoundValue();
	}
}

void ACreatureSystem::IncreaseAbsoluteAggressionLevel(float increase)
{
	ensureMsgf(increase >= 0, TEXT("The 'increase' value has to be larger than or equal 0"));
	
	if (!HasAuthority())
	{
		ServerIncreaseAbsoluteAggressionLevel(increase);
		return;
	}

	AbsoluteAggressionLevel += increase;
	if (!AggressionLevelRange.Contains(AbsoluteAggressionLevel))
	{
		AbsoluteAggressionLevel = AggressionLevelRange.GetUpperBoundValue();
	}
}

void ACreatureSystem::WatchingCreature(AActor* Watcher)
{
	if (!HasAuthority())
	{
		ServerWatchingCreature(Watcher);
		return;
	}
	
	if (!Watchers.Contains(Watcher))
		Watchers.AddUnique(Watcher);
		
	if (!CreatureBeingWatched)
	{
		CreatureBeingWatched = true;
		OnCreatureBeingWatched.Broadcast();
	}
}

void ACreatureSystem::StoppedWatchingCreature(AActor* Watcher)
{
	if (!HasAuthority())
	{
		ServerStoppedWatchingCreature(Watcher);
		return;
	}
	
	if (Watchers.Contains(Watcher))
		Watchers.Remove(Watcher);
		
	if (CreatureBeingWatched && Watchers.Num() == 0)
	{
		CreatureBeingWatched = false;
		OnCreatureStopBeingWatched.Broadcast();
	}
}

void ACreatureSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACreatureSystem, Creature);
	DOREPLIFETIME(ACreatureSystem, AggressionLevel);
	DOREPLIFETIME(ACreatureSystem, AbsoluteAggressionLevel);
	DOREPLIFETIME(ACreatureSystem, Hunting);
	DOREPLIFETIME(ACreatureSystem, CreatureBeingWatched);
}

void ACreatureSystem::ServerWatchingCreature_Implementation(AActor* Watcher)
{
	WatchingCreature(Watcher);
}

void ACreatureSystem::ServerStoppedWatchingCreature_Implementation(AActor* Watcher)
{
	StoppedWatchingCreature(Watcher);
}

void ACreatureSystem::ServerIncreaseAbsoluteAggressionLevel_Implementation(float increase)
{
	IncreaseAbsoluteAggressionLevel(increase);
}

void ACreatureSystem::ServerDecreaseAbsoluteAggressionLevel_Implementation(float decrease)
{
	DecreaseAbsoluteAggressionLevel(decrease);
}

void ACreatureSystem::ServerIncreaseAggressionLevel_Implementation(float increase)
{
	IncreaseAggressionLevel(increase);
}

void ACreatureSystem::ServerDecreaseAggressionLevel_Implementation(float decrease)
{
	DecreaseAggressionLevel(decrease);
}
