// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Item/AcousticSample.h"
#include "SynthComponents/EpicSynth1Component.h"
#include "EnemyVoice.generated.h"

USTRUCT(BlueprintType)
struct FAcousticBeat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAcousticSample> Beat;
};

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEASTINTHELABYRINTH_API UEnemyVoice : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyVoice(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void EnterArea(int32 playerId);

	UFUNCTION(BlueprintCallable)
	void ExitArea(int32 playerId);
	
	UPROPERTY()
	UModularSynthComponent *Synth;

	UPROPERTY(EditAnywhere, Category = "Behaviour", meta = (ClampMin = "0.2", ClampMax = "10", UIMin = "0.2", UIMax = "10"))
	float CheckRateInSeconds = 2;

	UPROPERTY(EditAnywhere, Category = "Behaviour", meta = (ClampMin = "3", ClampMax = "120", UIMin = "3", UIMax = "120"))
	float PlayerInAreaNeededToTriggerRhythm = 6;

	UPROPERTY(EditAnywhere, Category = "Behaviour", meta = (ClampMin = "5", ClampMax = "240", UIMin = "5", UIMax = "240"))
	float Cooldown = 30;

	
	UPROPERTY(EditAnywhere, Category = "Synth Sound")
	FModularSynthPreset SynthSettings;

	UPROPERTY(EditAnywhere, Category = "Synth Sound")
	TArray<FAcousticBeat> BeatTypes;

	UPROPERTY(VisibleAnywhere, Category = "Synth Sound", AdvancedDisplay)
	TArray<FAcousticSample> Rhythm;
	
	UPROPERTY(EditAnywhere, Category = "Synth Sound", meta = (ClampMin = "50", ClampMax = "1000", UIMin = "50", UIMax = "1000"))
	float SampleDurationMS = 200;
	
	UPROPERTY(EditAnywhere, Category = "Synth Sound", meta = (ClampMin = "15", ClampMax = "100", UIMin = "15", UIMax = "100"))
	float Note = 25;
	
protected:
	virtual void BeginPlay() override;

private:
	
	void GenerateRhythm();
	void PlayRhythm();
	void PlayNote();
	void AddBeat(int32 startIndex, int32 endIndex);

	void Check();
	
	FTimerHandle checkHandler;
	FTimerHandle noteHandler;
	int32 currentIndex = 0;
	TMap<int32, float> proximityPlayers;
	float rhythmPlayedTimestamp = 0;
	
	//int32 delay = 0;
};
