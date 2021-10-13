// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Game/MazeActor/IsleActor.h"
#include "UObject/NoExportTypes.h"
#include "Isle.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API UIsle : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY();
	FIntPoint Position;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> DoorsLeft;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> DoorsTop;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> DoorsRight;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> DoorsBottom;

	UPROPERTY(EditDefaultsOnly)
	FColor DebugColor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AIsleActor> IsleActor;

	UPROPERTY(EditDefaultsOnly)
	FIntPoint Size = FIntPoint(2, 2);

	UIsle();
};
