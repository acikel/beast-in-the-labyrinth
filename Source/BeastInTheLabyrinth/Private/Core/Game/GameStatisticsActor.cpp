// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/GameStatisticsActor.h"

#include "Core/Game/BeastGameInstance.h"
#include "Core/Game/LabyrinthGameMode.h"
#include "Core/Player/BeastPlayerState.h"
#include "Core/Player/PlayerCharacter.h"
#include "GameFramework/GameMode.h"
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
		}
	}
}

