// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectiveInterface.h"
#include "UObject/NoExportTypes.h"
#include "Objective.generated.h"


class AIsleActor;
//DECLARE_EVENT(UObjective, FOnCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompleted);


UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API UObjective : public UObject
{
	GENERATED_BODY()
	
public:
	// Called by game-mode or maze-generator after the actors are placed
	void Init(TArray<TScriptInterface<IObjectiveInterface>> Actors);

	UFUNCTION(BlueprintCallable, Category="Objectives")
	void CheckState();
	
	UFUNCTION(BlueprintCallable, Category="Objectives")
	bool IsCompleted() const { return bIsCompleted; }

	UPROPERTY(BlueprintAssignable, Category="Objectives")
	FOnCompleted OnCompleted;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives")
	FText DisplayedDescription;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives", meta = (MustImplement = "ObjectiveInterface"))
	TArray<TSubclassOf<AActor>> RequiredActors;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives")
	TArray<TSubclassOf<AIsleActor>> RequiredIsle;
	
protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Objectives")
	TArray<TScriptInterface<IObjectiveInterface>> GeneratedActors;

	UFUNCTION(BlueprintImplementableEvent)
	bool OnIsCompleted() const;

private:
	bool bIsCompleted = false;
};
