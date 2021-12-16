// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileActorSpawnInfo.h"
#include "GameFramework/Actor.h"
#include "TileActor.generated.h"

UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API ATileActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATileActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WallWest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WallNorth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WallEast;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WallSouth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PillarNorthWest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PillarNorthEast;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PillarSouthWest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PillarSouthEast;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTileActorSpawnInfo SpawnInfo;
	
	UPROPERTY(VisibleAnywhere)
	int32 TileValue;

	int32 GetId() { return Id; }
	
	void Init(const class UTile* Tile, const FTileActorSpawnInfo NewSpawnInfo);

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostLoad() override;

	UPROPERTY(VisibleAnywhere)
	int32 Id;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void DisableMeshComponent(UStaticMeshComponent* Wall);
	void Init();
	
};
