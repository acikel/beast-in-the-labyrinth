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
	const ABeastPlayerState* PS = PlayerController->GetPlayerState<ABeastPlayerState>();
	const uint32 PlayerId = PlayerController->GetPlayerState<ABeastPlayerState>()->GetPlayerId();
	
	FString PlayerName = PS->GetPlayerName();
	
	GameInstance->GameStatistics.PlayerStatistics.Add(PlayerId, FPlayerStatistics(PlayerId, PlayerName));
	TrackedPlayers.Add(PlayerController);
}

void AGameStatisticsActor::RecordPlayerDied(APlayerController* PlayerController)
{
	if (!HasAuthority())
	{
		ServerRecordPlayerDied(PlayerController);
		return;
	}

	const uint32 Id = PlayerController->GetPlayerState<ABeastPlayerState>()->GetPlayerId();
	GameInstance->GameStatistics.PlayerStatistics[Id].AmountOfDeaths++;
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
		const uint32 Id = PS->GetPlayerId();
		GameInstance->GameStatistics.PlayerStatistics[Id].AmountOfRevivesGiven++;
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
	GameInstance = Cast<UBeastGameInstance>(GetGameInstance());
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

	for (const auto PlayerController : TrackedPlayers)
	{
		APlayerCharacter* Pawn = Cast<APlayerCharacter>(PlayerController->GetPawn());

		if (Pawn)
		{
			int32 TileId = MazeGenerator->GetTileIdAtLocation(Pawn->GetActorLocation());
			const int32 PlayerId = PlayerController->GetPlayerState<ABeastPlayerState>()->GetPlayerId();
			
			GameInstance->GameStatistics.PlayerStatistics[PlayerId].CoveredTiles.AddUnique(TileId);
			//GEngine->AddOnScreenDebugMessage(4444, 5.0f, FColor::Green, FString::Printf(TEXT("Tile ID: %d"), TileId));
			
			const float Distance = FVector::Dist2D(Creature->GetActorLocation(), Pawn->GetActorLocation());
			if (Distance <= 1000)
			{
				GameInstance->GameStatistics.PlayerStatistics[PlayerId].TimeCloseToCreature += GetActorTickInterval();
			}
		}
	}
}

