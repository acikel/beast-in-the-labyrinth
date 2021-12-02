// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreatureSystem.generated.h"

UCLASS()
class BEASTINTHELABYRINTH_API ACreatureSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreatureSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	float AggressionLevel;

	UPROPERTY()
	FInt32Range AggressionLevelRange = FInt32Range(0, 1);
	

public:
	UFUNCTION(BlueprintCallable)
	float GetAggressionLevel() { return AggressionLevel; };

	UFUNCTION(BlueprintCallable)
	void IncreaseAggressionLevel(float increase)
	{
		ensureMsgf(increase >= 0, TEXT("The 'increase' value has to be larger than or equal 0"));
		
		AggressionLevel += increase;
		if (!AggressionLevelRange.Contains(AggressionLevel))
		{
			AggressionLevel = AggressionLevelRange.GetUpperBoundValue();
		}
	};

	UFUNCTION(BlueprintCallable)
	void DecreaseAggressionLevel(float decrease)
	{
		ensureMsgf(decrease >= 0, TEXT("The 'decrease' value has to be larger than or equal 0"));

		AggressionLevel -= decrease;
		if (!AggressionLevelRange.Contains(AggressionLevel))
		{
			AggressionLevel = AggressionLevelRange.GetLowerBoundValue();
		}
	};

};
