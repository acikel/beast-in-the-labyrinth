// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tile.generated.h"

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API UTile : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class UIsle* Origin;

	TArray<UTile*> Neighbours;

	
};
