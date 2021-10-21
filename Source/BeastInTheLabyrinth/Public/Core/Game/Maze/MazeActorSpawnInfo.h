#pragma once

#include "CoreMinimal.h"
#include "MazeActorSpawnInfo.generated.h"

UENUM(BlueprintType)
enum EActorDistributionType
{
	ABSOLUTE = 0,
	FIXED_AMOUNT = 1,
	DENSITY = 2,
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMazeActorSpawned, AActor*, Actor);

USTRUCT(BlueprintType)
struct FMazeActorSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistributionValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EActorDistributionType> DistributionType = FIXED_AMOUNT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInt32Range SpawnAmountOnTile = FInt32Range(1, 1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PlaceWithinDistanceToWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "PlaceWithinDistanceToWall"))
	FFloatRange DistanceToWall = FFloatRange(0, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform AbsoluteTransform = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnMazeActorSpawned OnMazeActorSpawned;
};
