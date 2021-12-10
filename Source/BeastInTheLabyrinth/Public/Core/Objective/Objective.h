// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectiveInterface.h"
#include "UObject/NoExportTypes.h"
#include "Objective.generated.h"


class UIsle;
class AIsleActor;

//DECLARE_EVENT(UObjective, FOnCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompleted);


UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API AObjective : public AInfo
{
	GENERATED_BODY()
	
public:
	AObjective();
	// Called by game-mode or maze-generator after the actors are placed
	// void OnRequiredActorsSpawned(TArray<TScriptInterface<IObjectiveInterface>> Actors);

	UFUNCTION(BlueprintCallable, Category="Objectives")
	void CheckState();
	
	UFUNCTION(BlueprintCallable, Category="Objectives")
	bool IsCompleted() const { return bIsCompleted; }

	UFUNCTION(BlueprintImplementableEvent)
	void OnPostGeneration();
	
	
	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category="Objectives")
	FText DisplayedDescription;

	UPROPERTY(BlueprintAssignable, Category="Objectives")
	FOnCompleted OnCompleted;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives", meta = (MustImplement = "ObjectiveInterface"))
	TArray<class UMazeActorSpawnInfo*> RequiredActors;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives")
	TArray<TSubclassOf<UIsle>> RequiredIsle;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives")
	TArray<TScriptInterface<IObjectiveInterface>> GeneratedActors;

	UFUNCTION(BlueprintImplementableEvent)
	bool OnIsCompleted() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCompleted();
	
private:
	UPROPERTY(Replicated)
	bool bIsCompleted = false;
};
