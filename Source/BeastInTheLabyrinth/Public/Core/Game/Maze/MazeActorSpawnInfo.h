#pragma once

#include "CoreMinimal.h"
#include "MazeActorSpawnInfo.generated.h"

UENUM(BlueprintType)
enum EActorDistributionType
{
	ABSOLUTE = 0,
	ABSOLUTE_AMOUNT = 1,
	DENSITY = 2,
};


USTRUCT(BlueprintType)
struct FMazeActorSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistributionValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EActorDistributionType> DistributionType = ABSOLUTE_AMOUNT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PlaceWithinDistanceToWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "PlaceWithinDistanceToWall"))
	float DistanceToWall = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform AbsoluteTransform = FTransform();
};
