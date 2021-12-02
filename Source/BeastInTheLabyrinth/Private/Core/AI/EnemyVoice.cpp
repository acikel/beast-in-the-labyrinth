// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AI/EnemyVoice.h"
#include "Kismet/KismetSystemLibrary.h"

UEnemyVoice::UEnemyVoice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEnemyVoice::BeginPlay()
{
	Super::BeginPlay();
	Synth = Cast<UModularSynthComponent>(GetOwner()->GetComponentByClass(UModularSynthComponent::StaticClass()));
	
	Synth->Activate();
	Synth->SetSynthPreset(SynthSettings);

	GenerateRhythm();
	
	GetWorld()->GetTimerManager().SetTimer(checkHandler, this, &UEnemyVoice::Check, CheckRateInSeconds, true, 0);
}

void UEnemyVoice::EnterArea(int32 playerId)
{
	float currentTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	proximityPlayers.FindOrAdd(playerId, currentTime);
}

void UEnemyVoice::ExitArea(int32 playerId)
{
	proximityPlayers.Remove(playerId);
}

void UEnemyVoice::Check()
{
	for (TTuple<int, float> proximityPlayer : proximityPlayers)
	{
		if(proximityPlayer.Value > PlayerInAreaNeededToTriggerRhythm)
		{
			if(UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) < rhythmPlayedTimestamp + Cooldown)
			{
				UE_LOG(LogTemp, Log, TEXT("Not ready to play rhythmus yet"));
				return;
			}
		
			rhythmPlayedTimestamp = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
			PlayRhythm();
			return;
		}
	}
}

void UEnemyVoice::GenerateRhythm()
{
	int32 beatCount = 4;

	AddBeat(0, 1);
	for (int32 b = 0; b < beatCount - 2; b++)
	{
		AddBeat(0, BeatTypes.Num() - 1);
	}
	AddBeat(2, BeatTypes.Num() - 1);
}

void UEnemyVoice::AddBeat(int32 startIndex, int32 endIndex)
{
	int32 randomIndex = FMath::RandRange(startIndex, endIndex);

	for (int32 n = 0; n < BeatTypes[randomIndex].Beat.Num(); n++)
	{
		Rhythm.Add(BeatTypes[randomIndex].Beat[n]);
	}
}


void UEnemyVoice::PlayRhythm()
{
	currentIndex = 0;
	PlayNote();
}

void UEnemyVoice::PlayNote()
{
	GetWorld()->GetTimerManager().ClearTimer(noteHandler);
	currentIndex++;
	
	if(currentIndex >= Rhythm.Num()) { return; }
	
	float durationInSeconds = Rhythm[currentIndex].ContinuousRepetitions * SampleDurationMS * 0.001f;
	if(Rhythm[currentIndex].NoteType > 0)
	{
		Synth->NoteOn(Note, 100, durationInSeconds);
	}
	
	GetWorld()->GetTimerManager().SetTimer(noteHandler, this, &UEnemyVoice::PlayNote, durationInSeconds + 0.1f, false);
}

