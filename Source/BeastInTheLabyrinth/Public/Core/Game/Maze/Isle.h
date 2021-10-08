// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Isle.generated.h"

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API UIsle : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TArray<UIsle*> ConnectedIsles;
};
