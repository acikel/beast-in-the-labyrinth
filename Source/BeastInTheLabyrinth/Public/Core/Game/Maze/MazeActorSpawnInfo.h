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

UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API UMazeActorSpawnInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange DistributionValue = FFloatRange(1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EActorDistributionType> DistributionType = FIXED_AMOUNT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInt32Range SpawnAmountOnTile = FInt32Range(1, 1);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PlaceWithinDistanceToWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "PlaceWithinDistanceToWall"))
	FFloatRange DistanceToWall = FFloatRange(50, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "PlaceWithinDistanceToWall"))
	FFloatRange RelativeZRotationToWall = FFloatRange(0.0f, 359.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform AbsoluteTransform = FTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnMazeActorSpawned OnMazeActorSpawned;
};
