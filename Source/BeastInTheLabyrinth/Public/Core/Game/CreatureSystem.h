// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreatureSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreatureAggressionLevelReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreatureCalmedDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreatureBeingWatched);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreatureStopBeingWatched);

UCLASS()
class BEASTINTHELABYRINTH_API ACreatureSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreatureSystem();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	float AggressionLevel;

	UPROPERTY()
	FInt32Range AggressionLevelRange = FInt32Range(0, 1);

	UPROPERTY(BlueprintReadOnly)
	AActor* Creature;

	UPROPERTY()
	bool Hunting;
	
	UPROPERTY()
	bool CreatureBeingWatched;

	UPROPERTY()
	TArray<AActor*> Watchers;

public:
	UFUNCTION(BlueprintCallable)
	float GetAggressionLevel() { return AggressionLevel; }

	UFUNCTION(BlueprintCallable)
	bool IsHunting() const { return Hunting; }

	UFUNCTION(BlueprintCallable)
	void IncreaseAggressionLevel(float increase)
	{
		ensureMsgf(increase >= 0, TEXT("The 'increase' value has to be larger than or equal 0"));
		
		AggressionLevel += increase;
		if (!AggressionLevelRange.Contains(AggressionLevel))
		{
			AggressionLevel = AggressionLevelRange.GetUpperBoundValue();
			if (!Hunting)
			{
				Hunting = true;
				OnCreatureAggressionLevelReached.Broadcast();
			}
		}
	};

	UFUNCTION(BlueprintCallable)
	void DecreaseAggressionLevel(float decrease)
	{
		ensureMsgf(decrease >= 0, TEXT("The 'decrease' value has to be larger than or equal 0"));

		AggressionLevel -= decrease;
		if (!AggressionLevelRange.Contains(AggressionLevel))
		{
			AggressionLevel = AggressionLevelRange.GetLowerBoundValue();
			if (Hunting)
			{
				Hunting = false;
				OnCreatureCalmedDown.Broadcast();
			}
		}
	};

	UFUNCTION(BlueprintCallable)
	void WatchingCreature(AActor* Watcher)
	{
		if (!Watchers.Contains(Watcher))
			Watchers.AddUnique(Watcher);
		
		if (!CreatureBeingWatched)
		{
			CreatureBeingWatched = true;
			OnCreatureBeingWatched.Broadcast();
		}
	}

	UFUNCTION(BlueprintCallable)
	void StoppedWatchingCreature(AActor* Watcher)
	{
		if (Watchers.Contains(Watcher))
			Watchers.Remove(Watcher);
		
		if (CreatureBeingWatched && Watchers.Num() == 0)
		{
			CreatureBeingWatched = false;
			OnCreatureStopBeingWatched.Broadcast();
		}
	}

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetWatchers() const { return Watchers; }

	UFUNCTION(BlueprintCallable)
	void RegisterCreature(AActor* NewCreature) { Creature = NewCreature; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetCreature() const { return Creature; };

	UPROPERTY(BlueprintAssignable)
	FOnCreatureAggressionLevelReached OnCreatureAggressionLevelReached;

	UPROPERTY(BlueprintAssignable)
	FOnCreatureCalmedDown OnCreatureCalmedDown;

	UPROPERTY(BlueprintAssignable)
	FOnCreatureBeingWatched OnCreatureBeingWatched;

	UPROPERTY(BlueprintAssignable)
	FOnCreatureStopBeingWatched OnCreatureStopBeingWatched;

};
