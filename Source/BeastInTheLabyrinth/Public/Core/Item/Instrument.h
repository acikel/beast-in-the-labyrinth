// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioCaptureComponent.h"

#include "AcousticSample.h"
#include "Core/AI/EnemyVoice.h"
#include "Core/Inventory/Item.h"
#include "Core/Player/BeastPlayerController.h"
#include "Instrument.generated.h"

// namespace FMOD
// {
// 	namespace Studio
// 	{
// 		class EventInstance;
// 	}
// }
//
// class UFMODEvent;
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
	virtual void Destroyed() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipped();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewLoudNotePlayed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnNewMutedNotePlayed();

	UFUNCTION(BlueprintImplementableEvent)
	void OnVolumeChanged();

	//UFUNCTION(BlueprintImplementableEvent)
	//UEnemyVoice* FindEnemyVoice() const;

	// UPROPERTY(EditAnywhere, Category = "FMOD")
	// UFMODEvent* StartPlayingInstrumentEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FMOD")
	float PlayerVolumeBooster = 30.0f;

	// UPROPERTY()
	// class UFMODAudioComponent* AudioComponent;
	
	/** Specified in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SamplingSize = 0.1f;

	/** Min required loud notes to compare rhythm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MinRequiredLoudNotes = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AggressionLevelReduction = 0.3f;

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
	int LastRegisteredSampleType = 0;
		
	UPROPERTY(ReplicatedUsing=OnRep_CurrentVoiceVolume, VisibleAnywhere, BlueprintReadOnly)
	float CurrentVoiceVolume = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAudioCaptureComponent* AudioCapture;

private:
	UFUNCTION()
	void UseInstrument(APlayerCharacter* Character);

	UFUNCTION()
	void AcousticPlayerInput(float EnvelopeValue);

	UFUNCTION(Server, Reliable)
	void Server_Setup();

	UFUNCTION(Server, Unreliable)
	void Server_SetVolume(const float &Volume);

	void Setup() { Server_Setup(); }
	
	void CalculateSample(const float EnvelopeAverage);
	void AddSample(const uint8 SampleType);
	void DiscardSamples();

	UFUNCTION(Server, Reliable)
	void Server_Compare(const TArray<FAcousticSample> &PlayerSamples);
	
	UFUNCTION()
	void OnRep_CurrentVoiceVolume();

	
	bool CompareRhythms(TArray<FAcousticSample> PlayerRhythm, TArray<FAcousticSample> OpponentRhythm, float& Score);
	void RemoveFirstAndLastEmpty(TArray<FAcousticSample> &PlayerRhythm);
	bool NormalizeRhythm(TArray<FAcousticSample> &Rhythm);
	void PrintRhythm(const TArray<FAcousticSample> &Rhythm, float TimeToDisplay = 3);
	

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

	// FMOD::Studio::EventInstance* InstrumentSound;

	FTimerHandle AbortTimerHandle;
	TArray<FAcousticSample> Local_Samples;
};

