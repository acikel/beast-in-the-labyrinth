// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AI/EnemyVoice.h"

#include "Core/BeastBlueprintFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UEnemyVoice::UEnemyVoice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UEnemyVoice::BeginPlay()
{
	Super::BeginPlay();
	
	Synth = Cast<UModularSynthComponent>(GetOwner()->GetComponentByClass(UModularSynthComponent::StaticClass()));
	Synth->Activate();
	Synth->SetSynthPreset(SynthSettings);

	if(GetOwner()->HasAuthority())
	{
		GenerateRhythm();
		GetWorld()->GetTimerManager().SetTimer(checkHandler, this, &UEnemyVoice::Check, CheckRateInSeconds, true, 3);
	}
}


// void UEnemyVoice::EnterArea(int32 playerId)
// {
// 	float currentTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
// 	proximityPlayers.FindOrAdd(playerId, currentTime);
// }
//
// void UEnemyVoice::ExitArea(int32 playerId)
// {
// 	proximityPlayers.Remove(playerId);
// }

void UEnemyVoice::Check()
{
	if(CreatureSystem == nullptr)
	{
		CreatureSystem = UBeastBlueprintFunctionLibrary::GetCreatureSystem(GetWorld());
		CreatureSystem->OnCreatureAggressionLevelReached.AddDynamic(this, &UEnemyVoice::OnAggressionLevelReached);
	}

	if(CreatureSystem->IsHunting()) { return; }
	
	TArray<APlayerState*> players = UGameplayStatics::GetGameMode(GetWorld())->GameState->PlayerArray;
	
	for (APlayerState* player : players)
	{
		if(player == nullptr || player->GetPawn() == nullptr) { continue; }
		
		float dist = 0;
		if(CreatureSystem != nullptr && CreatureSystem->GetCreature() != nullptr)
		{
			dist = FVector::Dist(player->GetPawn()->GetActorLocation(), CreatureSystem->GetCreature()->GetActorLocation());
		}
		
		if(dist <= AreaRadius)
		{
			if(UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()) < rhythmPlayedTimestamp + Cooldown)
			{
				UE_LOG(LogTemp, Log, TEXT("Not ready to play rhythmus yet"));
				return;
			}
		
			rhythmPlayedTimestamp = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
			AlreadyPlayedRepeats = 0;
			
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
	Multicast_PlayRhythm();
}

void UEnemyVoice::Multicast_PlayRhythm_Implementation()
{
	currentIndex = 0;
	PlayNote();
}


void UEnemyVoice::PlayNote()
{
	GetWorld()->GetTimerManager().ClearTimer(noteHandler);
	currentIndex++;
	
	if(currentIndex >= Rhythm.Num())
	{
		// Finished playing
		if(AlreadyPlayedRepeats < AdditionalRepeats)
		{
			// Repeat rhythm
			AlreadyPlayedRepeats++;
			GetWorld()->GetTimerManager().SetTimer(noteHandler, this, &UEnemyVoice::PlayRhythm, SilentTimeBetweenRepeats, false);
		}
		return;
	}
	
	float durationInSeconds = Rhythm[currentIndex].ContinuousRepetitions * SampleDurationMS * 0.001f;
	if(Rhythm[currentIndex].NoteType > 0)
	{
		Synth->NoteOn(Note, 100, durationInSeconds);
	}
	
	GetWorld()->GetTimerManager().SetTimer(noteHandler, this, &UEnemyVoice::PlayNote, durationInSeconds + 0.1f, false);
}

void UEnemyVoice::OnAggressionLevelReached()
{
	GetWorld()->GetTimerManager().ClearTimer(noteHandler);
	Synth->NoteOff(Note, true);
}
