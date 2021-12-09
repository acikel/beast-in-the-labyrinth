// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Objective/Objective.h"

#include "Net/UnrealNetwork.h"



AObjective::AObjective()
{
	bReplicates = true;
	bAlwaysRelevant = true;	// Find out that it is important for AInfo
}

void AObjective::CheckState()
{
	bIsCompleted = OnIsCompleted();
	if(bIsCompleted)
	{
		MulticastCompleted();
	}
}

void AObjective::MulticastCompleted_Implementation()
{
	OnCompleted.Broadcast();
}

void AObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AObjective, DisplayedDescription);
	DOREPLIFETIME(AObjective, bIsCompleted);
}



// void UObjective::OnRequiredActorsSpawned(TArray<TScriptInterface<IObjectiveInterface>> Actors)
// {
// 	GeneratedActors = Actors;
// 	
// 	for (const auto GeneratedActor : Actors)
// 	{
// 		
// 		//auto* ReactingObjectA = Cast<IObjectiveInterface>(GeneratedActor);
// 		//ReactingObjectA->OnInit(this);
// 		GeneratedActor->OnInit(this);
// 	}
// }
