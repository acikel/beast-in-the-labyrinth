// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AcousticSample.h"
#include "Core/Inventory/Item.h"
#include "Instrument.generated.h"

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API AInstrument : public AItem
{
	GENERATED_BODY()

	AInstrument();

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	bool CompareRhythms(TArray<FAcousticSample> PlayerRhythm, TArray<FAcousticSample> OpponentRhythm, float& score);

private:
	void RemoveFirstAndLastEmpty(TArray<FAcousticSample> &PlayerRhythm);
	bool NormalizeRhythm(TArray<FAcousticSample> &Rhythm);
};
