// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Item/Instrument.h"

// #include "FMODBlueprintStatics.h"
#include "Kismet/GameplayStatics.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"
#include "Core/BeastBlueprintFunctionLibrary.h"
#include "Core/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"


AInstrument::AInstrument()
	:AItem()
{
	AudioCapture = CreateDefaultSubobject<UAudioCaptureComponent>("AudioCaptureComponent");
	AudioCapture->SetupAttachment(RootComponent);
	AudioCapture->bEnableBaseSubmix = false;
	AudioCapture->bEnableBusSends = false;
	AudioCapture->bEnableSubmixSends = false;
}

void AInstrument::BeginPlay()
{
	Super::BeginPlay();
	
	OnUse.AddDynamic(this, &AInstrument::UseInstrument);

	if(GetLocalRole() == ROLE_Authority)
	{
		FTimerHandle setupHandler;
		GetWorld()->GetTimerManager().SetTimer(setupHandler, this, &AInstrument::Setup, 5, false);
	}

	// if(StartPlayingInstrumentEvent != nullptr)
	// {
	// 	// AudioComponent = UFMODBlueprintStatics::PlayEventAttached(
	// 	// 	StartPlayingInstrumentEvent, RootComponent, "FMOD_InstrumentPlayingSound",
	// 	// 	GetActorLocation(), EAttachLocation::KeepWorldPosition, true,
	// 	// 	false, false);
	// 	// AudioComponent->Activate();
	// }
	// else
	// {
	// 	UE_LOG(BeastGame, Error, TEXT("InstrumentPlayingSound FMOD Event not set within instrument."));
	// }
	
}

void AInstrument::Destroyed()
{
	Super::Destroyed();

	OnUse.RemoveDynamic(this, &AInstrument::UseInstrument);

	// if(InstrumentSound != nullptr)
	// {
	// 	InstrumentSound->release();
	//
	// 	//AudioComponent->Release();
	// }
}

void AInstrument::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInstrument, CreaturePawn);
	//DOREPLIFETIME(AInstrument, CurrentVoiceVolume);
	DOREPLIFETIME_CONDITION(AInstrument, CurrentVoiceVolume, COND_SkipOwner);
}

void AInstrument::OnRep_CurrentVoiceVolume()
{
	OnVolumeChanged();
}

void AInstrument::UseInstrument(APlayerCharacter* Character)
{
	UAudioCaptureComponent* PlayerAudioCapture = Cast<UAudioCaptureComponent>(Character->GetComponentByClass(UAudioCaptureComponent::StaticClass()));
	PlayerController = Character->GetController<ABeastPlayerController>();
	
	if(AudioCapture == nullptr)
	{
		UE_LOG(BeastGame, Error, TEXT("AudioCapture is null"));
		return;
	}
	
	ReadyToPlay = !ReadyToPlay;
	if(ReadyToPlay)
	{
		OnEquipped();
		PlayerController->ToggleSpeaking(false);
		PlayerAudioCapture->SetActive(false);
		AudioCapture->SetActive(true);
		
		AudioCapture->OnAudioEnvelopeValue.AddDynamic(this, &AInstrument::AcousticPlayerInput);
	}
	else
	{
		OnUnequipped();
		PlayerController->ToggleSpeaking(true);
		PlayerAudioCapture->SetActive(true);
		AudioCapture->SetActive(false);
		
		AudioCapture->OnAudioEnvelopeValue.RemoveDynamic(this, &AInstrument::AcousticPlayerInput);
	}
}


void AInstrument::AcousticPlayerInput(float EnvelopeValue)
{
	AcousticInputCounter++;
	EnvelopeSum += EnvelopeValue;
	
	CurrentVoiceVolume = FMath::Clamp(EnvelopeValue * PlayerVolumeBooster, 0.0f, 1.0f);
	Server_SetVolume(CurrentVoiceVolume);

	if(EnvelopeValue > SampleVolumeThreshold && LastRegisteredSampleType != 1)
	{
		OnNewLoudNotePlayed();
	}
	
	if(EnvelopeValue < SampleVolumeThreshold && LastRegisteredSampleType != 0)
	{
		OnNewMutedNotePlayed();
	}
	
	//AudioComponent->SetVolume(CurrentVoiceVolume); // A linear volume level. 0.0 = silent, 1.0 = full volume. Default = 1.0

	// if(InstrumentSound != nullptr) { InstrumentSound->setVolume(CurrentVoiceVolume); }
	
	float currentTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
	if(currentTime > NextSampleCalculation)
	{
		CalculateSample(EnvelopeSum / AcousticInputCounter);

		float distanceToCreature = 0;
		if(CreaturePawn != nullptr)
		{
			distanceToCreature = FVector::Dist(PlayerController->GetPawn()->GetActorLocation(), CreaturePawn->GetActorLocation());
		}
		
		if(NumIndividualLoudNotes >= MinRequiredLoudNotes && distanceToCreature < MaxDistanceForEffect)
		{
			Server_Compare(Local_Samples);
		}

		PrintRhythm(Local_Samples, SamplingSize);
		
		AcousticInputCounter = 0;
		EnvelopeSum = 0;
		NextSampleCalculation = currentTime + SamplingSize;
	}
}

void AInstrument::Server_SetVolume_Implementation(const float &Volume)
{
	CurrentVoiceVolume = Volume;
	OnRep_CurrentVoiceVolume();
}

void AInstrument::Server_Setup_Implementation()
{
	Server_CreatureSystem = UBeastBlueprintFunctionLibrary::GetCreatureSystem(GetWorld());
	CreaturePawn = Server_CreatureSystem->GetCreature();

	if(CreaturePawn != nullptr)
	{
		Server_CreatureVoice = Cast<UEnemyVoice>(CreaturePawn->GetComponentByClass(UEnemyVoice::StaticClass()));
	}
	
	if(Server_CreatureVoice == nullptr)
	{
		UE_LOG(BeastGame, Error, TEXT("Could not find a UEnemyVoice component assigned to the creature"));
		//SetActorEnableCollision(false);
		//SetActorHiddenInGame(true);
	}
}

void AInstrument::CalculateSample(const float EnvelopeAverage)
{
	const bool itSounds = EnvelopeAverage > SampleVolumeThreshold;
	LastRegisteredSampleType = itSounds ? 1 : 0;
	
	// Check if recorded samples can be discarded
	if(itSounds)
	{
		NumMutedSamples = 0;
	}
	else
	{
		NumMutedSamples++;
		if(NumMutedSamples >= MaxNumMutedSamplesTillDiscard)
		{
			DiscardSamples();
			return;
		}
	}

	// Add sample to the others
	if(Local_Samples.Num() > 0)
	{
		if(Local_Samples.Last().NoteType == LastRegisteredSampleType)
		{
			Local_Samples.Last().ContinuousRepetitions++;
		}
		else
		{
			AddSample(itSounds);
		}
	}
	else
	{
		AddSample(itSounds);
	}
}

void AInstrument::AddSample(const uint8 SampleType)
{
	FAcousticSample Sample;
	Sample.ContinuousRepetitions += 1;
	Sample.NoteType = SampleType;
	Local_Samples.Add(Sample);

	if(SampleType > 0)
	{
		NumIndividualLoudNotes++;
		// FFMODEventInstance InstanceWrapper = UFMODBlueprintStatics::PlayEventAtLocation(
		// 	GetWorld(), StartPlayingInstrumentEvent, FTransform(GetActorLocation()), true);
		//InstrumentSound = InstanceWrapper.Instance;

		//InstrumentSound->start();
	}
	else
	{
		//OnNewMutedNotePlayed();
		OnNewMutedNotePlayed();
		//InstrumentSound->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
}

void AInstrument::DiscardSamples()
{
	Local_Samples.Empty();
	NumMutedSamples = 0;
	NumIndividualLoudNotes = 0;
}

void AInstrument::Server_Compare_Implementation(const TArray<FAcousticSample> &PlayerSamples)
{
	if(Server_CreatureVoice == nullptr || Server_CreatureSystem == nullptr) { return; }
	
	float score = 0;
	const bool bIsValid = CompareRhythms(PlayerSamples, Server_CreatureVoice->Rhythm, score);

	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Silver,
			FString::Printf(TEXT("Rhythm score = %f - Passed = %s"),
				score, bIsValid ? TEXT("true") : TEXT("false")));

	if(bIsValid)
	{
		score *= AggressionLevelReduction;
		if (score > 0)
		{
			Server_CreatureSystem->DecreaseAggressionLevel(score);
		}
	}
	else
	{
		UE_LOG(BeastGame, Log, TEXT("Rhythm invalid: score = %f"), score);
	}
}

bool AInstrument::CompareRhythms(TArray<FAcousticSample> PlayerRhythm,
                                 TArray<FAcousticSample> OpponentRhythm, float& Score)
{
	UE_LOG(BeastGame, Log, TEXT("CompareRhythms"));

	//PrintRhythm(OpponentRhythm);

	RemoveFirstAndLastEmpty(PlayerRhythm);
	
	int32 totalSamples = 0;
	for (auto sample : PlayerRhythm) { totalSamples += sample.ContinuousRepetitions; }
	UE_LOG(BeastGame, Log, TEXT("CompareRhythms totalSamples: %d"), totalSamples);
	
	int32 inaccuracy = FMath::RoundToInt( static_cast<float>(totalSamples) * 0.05f);
	UE_LOG(BeastGame, Log, TEXT("CompareRhythms inaccuracy: %d"), inaccuracy);
	PlayerRhythm.RemoveAll([inaccuracy](const FAcousticSample &Sample){
		return Sample.ContinuousRepetitions <= inaccuracy;
	});
	
	RemoveFirstAndLastEmpty(PlayerRhythm);
	RemoveFirstAndLastEmpty(OpponentRhythm);
	
	if (!NormalizeRhythm(PlayerRhythm)) { return false; }
	if (!NormalizeRhythm(OpponentRhythm)) { return false; }
	
	float toleranceInDuration = 0.25f;
	float toleranceInStartTime = 0.25f;
	float shift = 0;
	int32 notesAccepted = 0;
	int32 startIndex = 0;
	
	for (FAcousticSample &opponentSample : OpponentRhythm)
	{
		for (int32 i = 0; i < PlayerRhythm.Num(); ++i)
		{
			FAcousticSample &playerSample = PlayerRhythm[i];

			float deltaStarttime = FMath::Abs(playerSample.StartTime + shift - opponentSample.StartTime);
			float deltaDuration = FMath::Abs(playerSample.NormalizedLength - opponentSample.NormalizedLength);
			
			if (opponentSample.NoteType == playerSample.NoteType &&
				deltaStarttime <= toleranceInStartTime &&
				deltaDuration <= toleranceInDuration)
			{
				//UE_LOG(BeastGame, Log, TEXT("shift: %f"), shift);
				UE_LOG(BeastGame, Log, TEXT("DELTA Starttime: %f | Duration: %f"), deltaStarttime, deltaDuration);

				Score += opponentSample.NormalizedLength - deltaDuration;
				shift = (opponentSample.StartTime + opponentSample.NormalizedLength) - (playerSample.StartTime + playerSample.NormalizedLength);
				notesAccepted++;
				startIndex = i + 1;
				break;
			}
		}
	}

	return OpponentRhythm.Num() == notesAccepted;
}


void AInstrument::RemoveFirstAndLastEmpty(TArray<FAcousticSample> &PlayerRhythm)
{
	if(PlayerRhythm.Num() == 0) { return; }
	if(PlayerRhythm[0].NoteType == 0)
	{
		PlayerRhythm.RemoveAt(0);
	}

	if(PlayerRhythm.Num() > 0 && PlayerRhythm[PlayerRhythm.Num() - 1].NoteType == 0)
	{
		PlayerRhythm.RemoveAt(PlayerRhythm.Num() - 1);
	}
}

bool AInstrument::NormalizeRhythm(TArray<FAcousticSample> &Rhythm)
{
	//UE_LOG(BeastGame, Log, TEXT("Rhythm.Num(): %d"), Rhythm.Num());
	if(Rhythm.Num() == 0) { return false; }
	
	float sampleCount = 0;
	for (const FAcousticSample &sample : Rhythm)
	{
		sampleCount += static_cast<float>(sample.ContinuousRepetitions);
		//UE_LOG(BeastGame, Log, TEXT("sample.ContinuousRepetitions: %d"), sample.ContinuousRepetitions);
	}
	//UE_LOG(BeastGame, Log, TEXT("sampleCount: %f"), sampleCount);

	if(sampleCount == 0) { return false; }
	
	float nextStartTime = 0;
	for (FAcousticSample &sample : Rhythm)
	{
		sample.StartTime = nextStartTime;
		sample.NormalizedLength = static_cast<float>(sample.ContinuousRepetitions) / sampleCount;
		//UE_LOG(BeastGame, Log, TEXT("NormalizedLength: %f"), sample.NormalizedLength);

		nextStartTime += sample.NormalizedLength;
		//UE_LOG(BeastGame, Log, TEXT("nextStartTime: %f"), nextStartTime);
	}

	return true;
}

void AInstrument::PrintRhythm(const TArray<FAcousticSample> &Rhythm, float TimeToDisplay)
{
	FString text = "";
	for (const FAcousticSample &sample : Rhythm)
	{
		text += FString::Printf(TEXT("  %d [%d]"), sample.NoteType, sample.ContinuousRepetitions);
	}

	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, FColor::Cyan, text);
}
