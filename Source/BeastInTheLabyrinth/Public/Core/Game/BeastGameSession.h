// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "OnlineSubsystemSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "BeastGameSession.generated.h"

#define SEARCH_TERM_BEAST_GAME "BEAST_IN_THE_LABYRINTH"
#define NAME_BEAST_GAME FName(TEXT(SEARCH_TERM_BEAST_GAME))

struct FBeastGameParams
{
	FName SessionName;
	FString LobbyName;
	bool IsLAN;
	//bool IsPresence;
	TSharedPtr<const FUniqueNetId> UserId;

	FBeastGameParams()
		: SessionName(NAME_BEAST_GAME)
		, LobbyName(TEXT("Untitled Lobby"))
		, IsLAN(false)
		//, IsPresence(false)
	{ }
};

DECLARE_EVENT_TwoParams(ASecurityGameSession, FOnCreatePresenceSessionComplete, FName /*SessionName*/, bool /*WasSuccessful*/);
	
DECLARE_MULTICAST_DELEGATE_TwoParams(FBeastOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FBeastOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBeastOnJoinSessionComplete, bool, Successful);


UCLASS()
class BEASTINTHELABYRINTH_API ABeastGameSession : public AGameSession
{
	GENERATED_BODY()

	ABeastGameSession(const FObjectInitializer& ObjectInitializer);

public:
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, const FString& UserName, const FName InSessionName, const FString& MapName, bool IsLAN);
	
	void FindSessions(const bool bIsLanQuery, const int32 MaxSearchResults = 10);
	// void JoinSession(const int32 SessionsIndex);
	void JoinSession(const FOnlineSessionSearchResult& DesiredSession);
	//void DestroySession();
	
	EOnlineSessionState::Type GetSessionState() const;
	
	FBeastOnFindSessionsComplete OnFindSessionsCompleteEvent;
	FBeastOnJoinSessionComplete OnJoinGameSessionCompleteEvent;
	//FBeastOnDestroySessionComplete OnDestroySessionCompleteEvent;
	FOnCreatePresenceSessionComplete OnCreatePresenceSessionCompleteEvent;

protected:
	// Called by delegates
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	//void OnDestroySessionComplete(FName InSessionName, bool WasSuccessful);
	void OnEndSessionComplete(FName InSessionName, bool WasSuccessful);
	void OnFindSessionsCompleted(bool Successful);
	
	void OnJoinSessionCompleted(FName JoinedSessionName, EOnJoinSessionCompleteResult::Type Result);
	

private:
	static constexpr int32 DefaultMaxPlayerCount = 4;

	// Create session
	FBeastGameParams CurrentSessionParams;
	TSharedPtr<FOnlineSessionSettings> HostSettings;
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	// Find sessions
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	// Join session
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
		
	FDelegateHandle StartSessionCompleteDelegateHandle;
	
	
	// End and destroy
	FOnEndSessionCompleteDelegate EndSessionCompleteDelegate;
	FDelegateHandle EndSessionCompleteDelegateHandle;
	
	// FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	// FDelegateHandle DestroySessionCompleteDelegateHandle;
};
