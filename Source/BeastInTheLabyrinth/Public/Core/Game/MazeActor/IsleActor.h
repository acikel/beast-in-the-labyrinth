// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IsleActor.generated.h"

UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API AIsleActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIsleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
