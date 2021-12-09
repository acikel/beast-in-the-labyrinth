// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObjectiveInterface.generated.h"

class AObjective;


UINTERFACE(MinimalAPI)
class UObjectiveInterface : public UInterface
{
	GENERATED_BODY()
};



class BEASTINTHELABYRINTH_API IObjectiveInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Objectives")
	void OnInit(AObjective* Objective);
};
