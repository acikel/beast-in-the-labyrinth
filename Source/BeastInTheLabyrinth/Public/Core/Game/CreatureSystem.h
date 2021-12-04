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


private:
	UPROPERTY()
	FTimerHandle DebugAggressionTimer;

	void DebugSampleAggressionLevel();
	float DecreaseEasingAlpha = 0.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	FDebugFloatHistory DebugAggressionLevel;

	UPROPERTY(EditAnywhere, Category="Aggression Settings")
	float AbsoluteAggressionLevelIncrement = 0.001f;

	UPROPERTY(EditAnywhere, Category="Aggression Settings")
	float CalmDownTime = 8.f;

	UPROPERTY(BlueprintReadOnly)
	float AggressionLevel = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float AbsoluteAggressionLevel = 0.f;

	UPROPERTY()
	FInt32Range AggressionLevelRange = FInt32Range(0.f, 1.f);

	UPROPERTY(BlueprintReadOnly)
	AActor* Creature;

	UPROPERTY()
	bool Hunting;
	
	UPROPERTY()
	bool CreatureBeingWatched;

	UPROPERTY()
	TArray<AActor*> Watchers;

	virtual void Tick(float DeltaSeconds) override;

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
		}

		if (!Hunting && AggressionLevel == AggressionLevelRange.GetUpperBoundValue())
		{
			Hunting = true;
			OnCreatureAggressionLevelReached.Broadcast();
		}

		DecreaseEasingAlpha = 0;
	};

	UFUNCTION(BlueprintCallable)
	void DecreaseAggressionLevel(float decrease)
	{
		ensureMsgf(decrease >= 0, TEXT("The 'decrease' value has to be larger than or equal 0"));

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

	void DecreaseAbsoluteAggressionLevel(float decrease)
	{
		ensureMsgf(decrease >= 0, TEXT("The 'decrease' value has to be larger than or equal 0"));

		AbsoluteAggressionLevel -= decrease;
		if (!AggressionLevelRange.Contains(AbsoluteAggressionLevel))
		{
			AbsoluteAggressionLevel = AggressionLevelRange.GetLowerBoundValue();
		}
	}

	void IncreaseAbsoluteAggressionLevel(float increase)
	{
		ensureMsgf(increase >= 0, TEXT("The 'increase' value has to be larger than or equal 0"));

		AbsoluteAggressionLevel += increase;
		if (!AggressionLevelRange.Contains(AbsoluteAggressionLevel))
		{
			AbsoluteAggressionLevel = AggressionLevelRange.GetUpperBoundValue();
		}
	}

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



	UFUNCTION(BlueprintCallable)
	FDebugFloatHistory GetDebugAggressionLevelHistory() { return DebugAggressionLevel; }

};
