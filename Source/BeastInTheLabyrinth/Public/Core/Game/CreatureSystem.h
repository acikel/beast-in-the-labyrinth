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

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AggressionLevel = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AbsoluteAggressionLevel = 0.f;

	UPROPERTY()
	FInt32Range AggressionLevelRange = FInt32Range(0.f, 1.f);

	UPROPERTY(Replicated, BlueprintReadOnly)
	AActor* Creature;

	UPROPERTY(Replicated)
	bool Hunting;
	
	UPROPERTY(Replicated)
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
	void IncreaseAggressionLevel(float increase);

	UFUNCTION(BlueprintCallable)
	void DecreaseAggressionLevel(float decrease);

	void DecreaseAbsoluteAggressionLevel(float decrease);

	void IncreaseAbsoluteAggressionLevel(float increase);

	UFUNCTION(BlueprintCallable)
	void WatchingCreature(AActor* Watcher);

	UFUNCTION(BlueprintCallable)
	void StoppedWatchingCreature(AActor* Watcher);

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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION(Server, Reliable)
	void ServerWatchingCreature(AActor* Watcher);
	
	UFUNCTION(Server, Reliable)
	void ServerStoppedWatchingCreature(AActor* Watcher);

	UFUNCTION(Server, Reliable)
	void ServerIncreaseAbsoluteAggressionLevel(float increase);

	UFUNCTION(Server, Reliable)
	void ServerDecreaseAbsoluteAggressionLevel(float decrease);
	
	UFUNCTION(Server, Reliable)
	void ServerIncreaseAggressionLevel(float increase);

	UFUNCTION(Server, Reliable)
	void ServerDecreaseAggressionLevel(float decrease);
};
