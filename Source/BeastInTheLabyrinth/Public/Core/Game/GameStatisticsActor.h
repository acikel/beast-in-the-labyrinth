// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStatistics.h"
#include "GameFramework/Actor.h"
#include "Maze/MazeGenerator.h"
#include "GameStatisticsActor.generated.h"

UCLASS()
class BEASTINTHELABYRINTH_API AGameStatisticsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameStatisticsActor();

	UFUNCTION()
	void SetGameStatistics(UGameStatistics* NewGameStatistics) { GameStatistics = NewGameStatistics; }

	void RegisterPlayerController(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void RecordPlayerDied(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void RecordPlayerRevived(APlayerController* PlayerController);
	
	UFUNCTION(Server, Reliable)
	void ServerRecordPlayerDied(APlayerController* PlayerController);

	UFUNCTION(Server, Reliable)
	void ServerRecordPlayerRevived(APlayerController* PlayerController);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UGameStatistics* GameStatistics;

	UPROPERTY()
	TArray<APlayerController*> TrackedPlayers;

private:
	UPROPERTY()
	AMazeGenerator* MazeGenerator;

	UPROPERTY()
	class ALabyrinthGameMode* GameMode;

	UPROPERTY()
	AActor* Creature;

	UFUNCTION()
	void Prepare();

	FTimerHandle WaitForGameModeHandle;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
