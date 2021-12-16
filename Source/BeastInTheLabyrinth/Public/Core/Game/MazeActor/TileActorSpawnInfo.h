#pragma once
#include "TileActorSpawnInfo.generated.h"

USTRUCT(BlueprintType)
struct FTileActorSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) int32 Id = 0;
	
	UPROPERTY(EditAnywhere)	bool Generated = false;

	UPROPERTY(EditAnywhere)	bool WallTop = true;
	UPROPERTY(EditAnywhere)	bool WallBottom = true;
	UPROPERTY(EditAnywhere)	bool WallLeft = true;
	UPROPERTY(EditAnywhere)	bool WallRight = true;
	
	UPROPERTY(EditAnywhere) bool PillarNorthWest = true;
	UPROPERTY(EditAnywhere) bool PillarNorthEast = true;
	UPROPERTY(EditAnywhere) bool PillarSouthWest = true;
	UPROPERTY(EditAnywhere) bool PillarSouthEast = true;

	bool IsValid() { return Generated; }
};
