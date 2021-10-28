// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FindSessionsCallbackProxy.h"
#include "Item/AcousticSample.h"
#include "BeastBlueprintFunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API UBeastBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Online|Session")
	static void GetSessionName(FBlueprintSessionResult SessionResult, FString& SessionName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameInstance")
	static FString ToString(ENetworkFailure::Type FailureType);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Flute")
	static float CompareRhythms(TArray<FAcousticSample> PlayerRhythm, TArray<FAcousticSample> OpponentRhythm);
};
