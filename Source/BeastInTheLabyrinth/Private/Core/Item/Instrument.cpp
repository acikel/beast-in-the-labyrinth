// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Item/Instrument.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"


AInstrument::AInstrument()
	:AItem()
{
}

bool AInstrument::CompareRhythms(TArray<FAcousticSample> PlayerRhythm,
                                 TArray<FAcousticSample> OpponentRhythm, float& score)
{
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
	
	float toleranceInDuration = 0.2f;
	float toleranceInStartTime = 0.2f;
	float shift = 0;
	int32 notesAccepted = 0;
	int32 startIndex = 0;
	
	for (FAcousticSample &opponentSample : OpponentRhythm)
	{
		for (int32 i = startIndex; i < PlayerRhythm.Num(); ++i)
		{
			FAcousticSample &playerSample = PlayerRhythm[i];

			float deltaStarttime = FMath::Abs(playerSample.StartTime + shift - opponentSample.StartTime);
			float deltaDuration = FMath::Abs(playerSample.NormalizedLength - opponentSample.NormalizedLength);
			
			if (opponentSample.NoteType == playerSample.NoteType &&
				deltaStarttime <= toleranceInStartTime &&
				deltaDuration <= toleranceInDuration)
			{
				UE_LOG(BeastGame, Log, TEXT("shift: %f"), shift);
				UE_LOG(BeastGame, Log, TEXT("DELTA Starttime: %f | Duration: %f"), deltaStarttime, deltaDuration);

				score += opponentSample.NormalizedLength - deltaDuration;
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

	if(PlayerRhythm[PlayerRhythm.Num() - 1].NoteType == 0)
	{
		PlayerRhythm.RemoveAt(PlayerRhythm.Num() - 1);
	}
}

bool AInstrument::NormalizeRhythm(TArray<FAcousticSample> &Rhythm)
{
	UE_LOG(BeastGame, Log, TEXT("Rhythm.Num(): %d"), Rhythm.Num());
	if(Rhythm.Num() == 0) { return false; }
	
	float sampleCount = 0;
	for (const FAcousticSample &sample : Rhythm)
	{
		sampleCount += static_cast<float>(sample.ContinuousRepetitions);
		UE_LOG(BeastGame, Log, TEXT("sample.ContinuousRepetitions: %d"), sample.ContinuousRepetitions);
	}
	UE_LOG(BeastGame, Log, TEXT("sampleCount: %f"), sampleCount);

	if(sampleCount == 0) { return false; }
	
	float nextStartTime = 0;
	for (FAcousticSample &sample : Rhythm)
	{
		sample.StartTime = nextStartTime;
		sample.NormalizedLength = static_cast<float>(sample.ContinuousRepetitions) / sampleCount;
		UE_LOG(BeastGame, Log, TEXT("NormalizedLength: %f"), sample.NormalizedLength);

		nextStartTime += sample.NormalizedLength;
		UE_LOG(BeastGame, Log, TEXT("nextStartTime: %f"), nextStartTime);
	}

	return true;
}

// OpponentRhythm.Sort([](const FAcousticSample& a, const FAcousticSample& b){
// 	return a.NormalizedLength > b.NormalizedLength;
// });
//
// for (auto sample : OpponentRhythm)
// {
// 	PlayerRhythm.Sort([&sample](const FAcousticSample& a, const FAcousticSample& b){
// 		return FMath::Abs(a.StartTime - sample.StartTime) > FMath::Abs(b.StartTime - sample.StartTime);
// 	});
// 	PlayerRhythm[0];
// }
// PlayerRhythm.Sort([&sample](const FAcousticSample& a, const FAcousticSample& b){
// 	return FMath::Abs(a.StartTime - sample.StartTime) > FMath::Abs(b.StartTime - sample.StartTime);
// });
// PlayerRhythm[0];
	