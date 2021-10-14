// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Objective/Objective.h"


void UObjective::Init(TArray<TScriptInterface<IObjectiveInterface>> Actors)
{
	GeneratedActors = Actors;
	
	for (const auto GeneratedActor : Actors)
	{
		
		//auto* ReactingObjectA = Cast<IObjectiveInterface>(GeneratedActor);
		//ReactingObjectA->OnInit(this);
		GeneratedActor->OnInit(this);
	}
}

void UObjective::CheckState()
{
	bIsCompleted = OnIsCompleted();
	if(bIsCompleted)
	{
		OnCompleted.Broadcast();
	}
}
