// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Game/BeastGameSession.h"
#include "BeastInTheLabyrinth/BeastInTheLabyrinth.h"

ABeastGameSession::ABeastGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ABeastGameSession::OnCreateSessionComplete);
		// DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ABeastGameSession::OnDestroySessionComplete);
		EndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &ABeastGameSession::OnEndSessionComplete);
		
		FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ABeastGameSession::OnFindSessionsCompleted);
		JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ABeastGameSession::OnJoinSessionCompleted);
	}
}

bool ABeastGameSession::HostSession(TSharedPtr<const FUniqueNetId> UserId, const FString& UserName, const FName InSessionName, const FString& MapName, bool IsLAN)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	UE_LOG(BeastGame, Log, TEXT("ABeastGameSession::HostSession"));
	
	if (OnlineSubsystem)
	{
		UE_LOG(BeastGame, Log, TEXT("has OnlineSubsystem"));
		
		CurrentSessionParams.SessionName = InSessionName;
		CurrentSessionParams.IsLAN = IsLAN;
		CurrentSessionParams.UserId = UserId;
		//CurrentSessionParams.IsPresence = false;

		const IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			HostSettings = MakeShareable(new FOnlineSessionSettings());
			HostSettings->bIsLANMatch = IsLAN;

			// If true: this session is discoverable by presence (Epic Games launcher & Epic Social Overlay).
			// For subsystems that supports presence... displaying information such as whether the player is in a lobby,
			// in matchmaking or in the middle of a game.
			HostSettings->bUsesPresence = true;
			
			HostSettings->NumPublicConnections = DefaultMaxPlayerCount;
			HostSettings->NumPrivateConnections = 0;
			HostSettings->bShouldAdvertise = true;
			HostSettings->bAllowJoinInProgress = true;
			HostSettings->bAllowInvites = true;
			HostSettings->bAllowJoinViaPresence = true;
			HostSettings->bAllowJoinViaPresenceFriendsOnly = false;
			HostSettings->bUseLobbiesIfAvailable = true;
			HostSettings->bUseLobbiesVoiceChatIfAvailable = false;

			const FString SessionTitle = FString::Printf(TEXT("%s's lobby created at %s"), *UserName, *FDateTime::Now().ToString());
			HostSettings->Set(FName("SESSION_NAME"), SessionTitle, EOnlineDataAdvertisementType::ViaOnlineService);

			HostSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
			
			
			FSessionSettings& UserSettings = HostSettings->MemberSettings.Add(UserId.ToSharedRef(), FSessionSettings());
			UserSettings.Add(SETTING_GAMEMODE, FOnlineSessionSetting(FString("GameSession"), EOnlineDataAdvertisementType::ViaOnlineService));
			
			//if (!(PLATFORM_SWITCH && IsLAN))
			//{
				HostSettings->Set(SEARCH_KEYWORDS, FString(SEARCH_TERM_BEAST_GAME), EOnlineDataAdvertisementType::ViaOnlineService);
			//}

			UE_LOG(BeastGame, Log, TEXT("Before CreateSession"));
			
			CreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
			return Sessions->CreateSession(*CurrentSessionParams.UserId, CurrentSessionParams.SessionName, *HostSettings);
		}
		else
		{
			OnCreateSessionComplete(InSessionName, false);
		}
	}
#if !UE_BUILD_SHIPPING
	else
	{
		// Hack workflow in development
		OnCreatePresenceSessionCompleteEvent.Broadcast(NAME_GameSession, true);
		return true;
	}
#endif

	return false;
}

void ABeastGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(BeastGame, Log, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
	if (OnlineSub)
	{
		const IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnCreatePresenceSessionCompleteEvent.Broadcast(InSessionName, bWasSuccessful);	
}



void ABeastGameSession::OnEndSessionComplete(FName InSessionName, bool WasSuccessful)
{
	UE_LOG(BeastGame, Log, TEXT("USecurityGameInstance::OnEndSessionComplete: Session: %s, WasSuccessful: %d"), *InSessionName.ToString(), WasSuccessful);
	
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteDelegateHandle);
	}
}


void ABeastGameSession::FindSessions(const bool bIsLanQuery, const int32 MaxSearchResults)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = bIsLanQuery;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		// Failed to find sessions. So unregister delegate
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void ABeastGameSession::OnFindSessionsCompleted(bool Successful)
{
	UE_LOG(BeastGame, Log, TEXT("ABeastGameSession::OnFindSessionsCompleted(Sucessful: %d)"), Successful);
	
	// Unregister
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsCompleted: Num Search Results: %d"), LastSessionSearch->SearchResults.Num());

	// If nothing was found
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		UE_LOG(BeastGame, Log, TEXT("No sessions found"));
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), Successful);
		return;
	}

	OnFindSessionsCompleteEvent.Broadcast(LastSessionSearch->SearchResults, Successful);
}



void ABeastGameSession::JoinSession(const FOnlineSessionSearchResult& DesiredSession)
{
	UE_LOG(BeastGame, Log, TEXT("ABeastGameSession::JoinSession"));
	
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, DesiredSession))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void ABeastGameSession::OnJoinSessionCompleted(FName JoinedSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(BeastGame, Log, TEXT("Session joined %s"), *JoinedSessionName.ToString());
	
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	
	OnJoinGameSessionCompleteEvent.Broadcast(Result);
}




EOnlineSessionState::Type ABeastGameSession::GetSessionState() const
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		return SessionInterface->GetSessionState(NAME_GameSession);
	}
	return EOnlineSessionState::NoSession;
}


// void ABeastGameSession::DestroySession()
// {
// 	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
// 	if (!SessionInterface.IsValid())
// 	{
// 		//OnDestroySessionCompleteEvent.Broadcast(false);
// 		return;
// 	}
//
// 	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
//
// 	if (!SessionInterface->DestroySession(NAME_GameSession))
// 	{
// 		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
// 		//OnDestroySessionCompleteEvent.Broadcast(false);
// 	}
// }
//
// void ABeastGameSession::OnDestroySessionComplete(FName InSessionName, bool WasSuccessful)
// {
// 	UE_LOG(BeastGame, Log, TEXT("OnDestroySessionComplete %s Success: %d"), *InSessionName.ToString(), WasSuccessful);
//
// 	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
// 	if (!SessionInterface.IsValid())
// 	{
// 		//OnDestroySessionCompleteEvent.Broadcast(false);
// 		return;
// 	}
//
// 	HostSettings = nullptr;
// 	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
// 	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
// 	SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
// 	//OnDestroySessionCompleteEvent.Broadcast(WasSuccessful);
// }
