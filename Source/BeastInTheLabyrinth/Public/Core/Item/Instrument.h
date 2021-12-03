// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioCaptureComponent.h"

#include "AcousticSample.h"
#include "Core/AI/EnemyVoice.h"
#include "Core/Inventory/Item.h"
#include "Core/Player/BeastPlayerController.h"
#include "Instrument.generated.h"

class ACreatureSystem;
/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API AInstrument : public AItem
{
	GENERATED_BODY()

public:
	AInstrument();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipped();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent)
	UEnemyVoice* FindEnemyVoice() const;
	

	/** Specified in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SamplingSize = 0.1f;

	/** Min required loud notes to compare rhythm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MinRequiredLoudNotes = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AggressionLevelReduction = -0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistanceForEffect = 1500;

	/** Min required loud notes to compare rhythm */
	UPROPERTY(EditAnywhere)
	int MaxNumMutedSamplesTillDiscard = 12;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SampleVolumeThreshold = 0.015f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ReadyToPlay = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentVoiceVolume = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int LastRegisteredSampleType = 0;

private:
	UFUNCTION()
	void UseInstrument(APlayerCharacter* Character);

	UFUNCTION()
	void AcousticPlayerInput(float EnvelopeValue);

	UFUNCTION(Server, Reliable)
	void Server_Setup();

	void Setup() { Server_Setup(); }
	
	void CalculateSample(const float EnvelopeAverage);
	void AddSample(const uint8 SampleType);
	void DiscardSamples();

	UFUNCTION(Server, Reliable)
	void Server_Compare(const TArray<FAcousticSample> &PlayerSamples);
	
	bool CompareRhythms(TArray<FAcousticSample> PlayerRhythm, TArray<FAcousticSample> OpponentRhythm, float& Score);
	void RemoveFirstAndLastEmpty(TArray<FAcousticSample> &PlayerRhythm);
	bool NormalizeRhythm(TArray<FAcousticSample> &Rhythm);
	void PrintRhythm(const TArray<FAcousticSample> &Rhythm, float TimeToDisplay = 3);
	
	UPROPERTY()
	UAudioCaptureComponent* AudioCapture;

	UPROPERTY()
	UEnemyVoice* Server_CreatureVoice;

	UPROPERTY()
	ACreatureSystem* Server_CreatureSystem;

	UPROPERTY(Replicated)
	AActor* CreaturePawn;

	UPROPERTY()
	ABeastPlayerController* PlayerController;
	
	int32 AcousticInputCounter = 0;
	float EnvelopeSum = 0;
	float NextSampleCalculation = 0;
	int NumMutedSamples = 0;
	int NumIndividualLoudNotes = 0;

	FTimerHandle AbortTimerHandle;
	TArray<FAcousticSample> Local_Samples;
};

