// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/BeastPlayerController.h"

#include "Core/Game/BeastGameInstance.h"
#include "Core/Game/LabyrinthGameMode.h"
#include "Core/Player/BeastPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABeastPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// const FString NewPlayerName = FString::Printf(TEXT("Mega%d"), FMath::RandRange(100, 999));
	// GetWorld()->GetFirstPlayerController()->PlayerState->SetPlayerName(*NewPlayerName);
	
	if (IsLocalPlayerController())
	{
		SetPlayerName();
	}
}

bool ABeastPlayerController::ServerChangePlayerName_Validate(const FString& NewPlayerName)
{
	RPC_VALIDATE( !NewPlayerName.IsEmpty() );
	return true;
}

void ABeastPlayerController::ServerChangePlayerName_Implementation(const FString& NewPlayerName)
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (!NewPlayerName.IsEmpty() && GameMode)
	{
		GameMode->ChangeName( this, NewPlayerName, true );
	}

	OnChangeName.Broadcast(this, NewPlayerName);
	RegisterStatistics();
}

void ABeastPlayerController::RegisterStatistics()
{
	ALabyrinthGameMode* GameMode = Cast<ALabyrinthGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		const bool valid = IsValid(PlayerState);
		ABeastPlayerState* PS = GetPlayerState<ABeastPlayerState>();
		if (PS)
		{
			GameMode->GetGameStatistics()->RegisterPlayerController(this);
		}
		else
		{
			if (!valid)
			{
				GetWorldTimerManager().SetTimer(RegisterStatisticsDelayHandle, this, &ABeastPlayerController::RegisterStatistics, 0.2f, false, 0.f);
			}
		}
	}
}

void ABeastPlayerController::SetPlayerName()
{
	UBeastGameInstance* GameInstance = Cast<UBeastGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		ABeastPlayerState* PS = GetPlayerState<ABeastPlayerState>();
		if (PS)
		{
			PS->SetPlayerName(GameInstance->GetProfilePlayerName());
			ServerChangePlayerName(GameInstance->GetProfilePlayerName());
		}
		else
		{
			GetWorldTimerManager().SetTimer(SetPlayerNameHandle, this, &ABeastPlayerController::SetPlayerName, 0.2f, false, 0.f);
		}
	}
}
