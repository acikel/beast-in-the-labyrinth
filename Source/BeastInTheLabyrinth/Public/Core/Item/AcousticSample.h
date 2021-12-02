// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AcousticSample.generated.h"

USTRUCT(BlueprintType)
struct FAcousticSample
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 NoteType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ContinuousRepetitions;

	UPROPERTY()
	float NormalizedLength = 0;
	
	UPROPERTY()
	float StartTime = 0;
};

