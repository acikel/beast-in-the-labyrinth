// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/GameStatisticsActor.h"

#include "Core/Game/BeastGameInstance.h"
#include "Core/Game/LabyrinthGameMode.h"
#include "Core/Player/BeastPlayerState.h"
#include "Core/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameStatisticsActor::AGameStatisticsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2f;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

void AGameStatisticsActor::RegisterPlayerController(APlayerController* PlayerController)
{
	ABeastPlayerState* PS = PlayerController->GetPlayerState<ABeastPlayerState>();
	
	GameStatistics->AddStatisticForPlayer(PlayerController->GetPlayerState<ABeastPlayerState>()->GetPlayerId(), PS->GetPlayerName());
	TrackedPlayers.Add(PlayerController);
}

void AGameStatisticsActor::RecordPlayerDied(APlayerController* PlayerController)
{
	if (!HasAuthority())
	{
		ServerRecordPlayerDied(PlayerController);
		return;
	}
	
	uint32 Id = PlayerController->GetPlayerState<ABeastPlayerState>()->GetPlayerId();
	GameStatistics->PlayerStatistics[Id].AmountOfDeaths++;
}

void AGameStatisticsActor::RecordPlayerRevived(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("RecordPlayerRevived: No PlayerController set to record Player revived."));
		return;
	}
	
	if (!HasAuthority())
	{
		ServerRecordPlayerRevived(PlayerController);
		return;
	}

	ABeastPlayerState* PS = PlayerController->GetPlayerState<ABeastPlayerState>();
	if (PS)
	{
		uint32 Id = PS->GetPlayerId();
		GameStatistics->PlayerStatistics[Id].AmountOfRevivesGiven++;
	}
}

void AGameStatisticsActor::ServerRecordPlayerDied_Implementation(APlayerController* PlayerController)
{
	RecordPlayerDied(PlayerController);
}

void AGameStatisticsActor::ServerRecordPlayerRevived_Implementation(APlayerController* PlayerController)
{
	RecordPlayerRevived(PlayerController);
}

// Called when the game starts or when spawned
void AGameStatisticsActor::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ALabyrinthGameMode>(UGameplayStatics::GetGameMode(this));
	Prepare();
}

void AGameStatisticsActor::Prepare()
{
	if (GameMode)
	{
		if (GameMode->IsReady())
		{
			MazeGenerator = GameMode->GetMazeGenerator();
			Creature = GameMode->GetCreatureSystem()->GetCreature();
			SetActorTickEnabled(true);
		}
		else
		{
			GetWorldTimerManager().SetTimer(WaitForGameModeHandle, this, &AGameStatisticsActor::Prepare, 0.2f);
		}
	}
}

// Called every frame
void AGameStatisticsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto PlayerController : TrackedPlayers)
	{
		APlayerCharacter* Pawn = Cast<APlayerCharacter>(PlayerController->GetPawn());

		if (Pawn)
		{
			int32 TileId = MazeGenerator->GetTileIdAtLocation(Pawn->GetActorLocation());
			const int32 PlayerId = PlayerController->GetPlayerState<ABeastPlayerState>()->GetPlayerId();
			
			GameStatistics->PlayerStatistics[PlayerId].CoveredTiles.AddUnique(TileId);
			//GEngine->AddOnScreenDebugMessage(4444, 5.0f, FColor::Green, FString::Printf(TEXT("Tile ID: %d"), TileId));
			
			float Distance = FVector::Dist2D(Creature->GetActorLocation(), Pawn->GetActorLocation());
			if (Distance <= 800)
			{
				GameStatistics->PlayerStatistics[PlayerId].TimeCloseToCreature += GetActorTickInterval();
			}
		}
	}
}

