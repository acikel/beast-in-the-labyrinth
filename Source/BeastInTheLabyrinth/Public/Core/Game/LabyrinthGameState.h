// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LabyrinthGameState.generated.h"

class AObjective;


UCLASS()
class BEASTINTHELABYRINTH_API ALabyrinthGameState : public AGameStateBase
{
	GENERATED_BODY()

	
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere)
	TArray<AObjective*> ChosenObjectives;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere)
	AActor* Creature;
};
