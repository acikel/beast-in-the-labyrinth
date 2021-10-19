// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/BeastGameInstance.h"

#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"



void UBeastGameInstance::Init()
{
	Super::Init();
	
	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UBeastGameInstance::OnDestroySessionComplete);
	TravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UBeastGameInstance::OnTravelLocalSessionFailure);
	NetworkFailureDelegateHandle = GEngine->OnNetworkFailure().AddUObject(this, &UBeastGameInstance::OnNetworkFailure);

	CVarAPEX
}

bool UBeastGameInstance::HostGame()
{
	UE_LOG(BeastGame, Log, TEXT("UBeastGameInstance::HostSession"));

	const ULocalPlayer* LocalPlayer = GetFirstGamePlayer();
	const FString GameMode = FString(TEXT("/Game/BeastInTheLabyrinth/Core/Game/BP_LobbyGameMode.BP_LobbyGameMode_C"));
	
	LevelName = TEXT("/Game/BeastInTheLabyrinth/Map/Lobby");
	LevelOptions = FString::Printf(TEXT("?game=%s%s"), *GameMode, TEXT("?listen"));

	
	ABeastGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		UE_LOG(BeastGame, Log, TEXT("Has Game Session"));
		CreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionCompleteEvent.AddUObject(this, &UBeastGameInstance::OnCreatePresenceSessionComplete);
		
		bool constexpr IsLanGame = false;
		UE_LOG(BeastGame, Log, TEXT("MapName: %s"), *LevelName);
		
		if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), *ProfilePlayerName, NAME_GameSession, *LevelName, IsLanGame))
		{
			return true;
		}
	}

	return false;
}


void UBeastGameInstance::OnCreatePresenceSessionComplete(FName SessionName, bool WasSuccessful)
{
	ABeastGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnCreatePresenceSessionCompleteEvent.Remove(CreatePresenceSessionCompleteDelegateHandle);
		FinishSessionCreation(WasSuccessful ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UBeastGameInstance::FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (GIsEditor)
		{
			UE_LOG(BeastGame, Log, TEXT("Open Level: %s, Options: %s"), *LevelName, *LevelOptions);
			UGameplayStatics::OpenLevel(GetWorld(), FName(LevelName), false, "listen" + LevelOptions);
		}
		else
		{
			UE_LOG(BeastGame, Log, TEXT("Open Level: %s, Options: %s"), *LevelName, *LevelOptions);

			FURL URL;
			URL.Map = LevelName;
			URL.AddOption(TEXT("listen"));

			FString Error;
			GEngine->Browse(*WorldContext, URL, Error);
			if (!Error.IsEmpty())
			{
				UE_LOG(BeastGame, Warning, TEXT("Failed to load level: %s"), *Error);
			}
		}
	}
	else
	{
		UE_LOG(BeastGame, Warning, TEXT("Failed to create session."));
	}
}



/** Initiates the session searching */
bool UBeastGameInstance::FindGames(const bool bFindLan)
{
	ABeastGameSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnFindSessionsCompleteEvent.RemoveAll(this);
		FindSessionsCompleteDelegateHandle = GameSession->OnFindSessionsCompleteEvent.AddUObject(this, &UBeastGameInstance::OnFindSessionsCompleted);

		GameSession->FindSessions(bFindLan);
		return true;
	}
	return false;
}


void UBeastGameInstance::OnFindSessionsCompleted(const TArray<FOnlineSessionSearchResult>& SessionResults, const bool bSuccessful)
{
	ABeastGameSession* const GameSession = GetGameSession();
	
	if (GameSession)
	{
		GameSession->OnFindSessionsCompleteEvent.Remove(FindSessionsCompleteDelegateHandle);
	}

	TArray<FBlueprintSessionResult> SessionList;
	for (int32 SearchIdx = 0; SearchIdx < SessionResults.Num(); SearchIdx++)
	{
		const FOnlineSessionSearchResult& SearchResult = SessionResults[SearchIdx];
		
		FBlueprintSessionResult BPSessionResult = FBlueprintSessionResult();
		BPSessionResult.OnlineResult = SearchResult;
		SessionList.Add(BPSessionResult);
	}

	OnSessionsFound.Broadcast(SessionList, bSuccessful);
}


void UBeastGameInstance::JoinGame(const FBlueprintSessionResult & Session)
{
	ABeastGameSession* const GameSession = GetGameSession();
	
	if (GameSession)
	{
		GameSession->OnJoinGameSessionCompleteEvent.RemoveAll(this);
		GameSession->OnJoinGameSessionCompleteEvent.AddUObject(this, &UBeastGameInstance::OnJoinCompleted);
		GameSession->JoinSession(Session.OnlineResult);
	}
}

void UBeastGameInstance::OnJoinCompleted(EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(BeastGame, Log, TEXT("Session joined with the result: %s"), LexToString(Result));
	
	ABeastGameSession* const GameSession = GetGameSession();
	
	if (GameSession)
	{
		GameSession->OnJoinGameSessionCompleteEvent.RemoveAll(this);
	}
	
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		ClientTravelToCurrentSession();
	}
}

void UBeastGameInstance::DestroySession()
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnDestroySessionCompleteEvent.Broadcast(false);
		return;
	}
	
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		OnDestroySessionCompleteEvent.Broadcast(false);
	}
}

void UBeastGameInstance::OnDestroySessionComplete(FName InSessionName, bool WasSuccessful)
{
	UE_LOG(BeastGame, Log, TEXT("OnDestroySessionComplete %s Success: %d"), *InSessionName.ToString(), WasSuccessful);

	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnDestroySessionCompleteEvent.Broadcast(false);
		return;
	}
	
	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	
	OnDestroySessionCompleteEvent.Broadcast(WasSuccessful);
}


void UBeastGameInstance::QuitToMenu()
{
	DestroySession();

	FURL Url;
	Url.Map = "MainMenu";
	Url.AddOption(TEXT("game=/Game/BeastInTheLabyrinth/Core/Game/BP_LobbyGameMode.BP_LobbyGameMode_C"));

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->ClientTravel(Url.ToString(), TRAVEL_Absolute);
}


void UBeastGameInstance::ClientTravelToCurrentSession() const
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		return;
	}

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString))
	{
		return;
	}

	UE_LOG(BeastGame, Log, TEXT("Try to travel to: '%s'"), *ConnectString);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
}

void UBeastGameInstance::OnTravelLocalSessionFailure(UWorld* World, ETravelFailure::Type FailureType,
													const FString& ErrorString)
{
	UE_LOG(BeastGame, Warning, TEXT("TravelFailure: %d ErrorString: %s"), static_cast<int32>(FailureType), *ErrorString);

	QuitToMenu();
}

void UBeastGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& Error)
{
	if (NetDriver)
	{
		UE_LOG(BeastGame, Error, TEXT("NetworkFailure NetDriver: %s, FailureType: %s, Error: %s"), *NetDriver->GetName(), ENetworkFailure::ToString(FailureType), *Error);		
	}
	else
	{
		UE_LOG(BeastGame, Error, TEXT("NetworkFailure FailureType: %s, Error: %s"), ENetworkFailure::ToString(FailureType), *Error);
	}
	
	QuitToMenu();
}




FString UBeastGameInstance::GetSessionState() const
{
	ABeastGameSession* const GameSession = GetGameSession();
	if(GameSession)
	{
		return EOnlineSessionState::ToString(GameSession->GetSessionState());
	}
	return TEXT("");
}

// This works not as client, because the game mode is only available on the server
ABeastGameSession* UBeastGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		
		if (Game)
		{
			return Cast<ABeastGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}

