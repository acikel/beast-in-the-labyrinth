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
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DistributionValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EActorDistributionType> DistributionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AbsoluteLocation;

	FMazeActorSpawnInfo()
	{
		DistributionValue = 10;
		DistributionType = ABSOLUTE;
		AbsoluteLocation = FVector();
	}

	
};
