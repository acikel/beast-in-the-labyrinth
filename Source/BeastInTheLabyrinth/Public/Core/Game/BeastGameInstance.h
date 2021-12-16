// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BeastGameSession.h"
#include "FindSessionsCallbackProxy.h"
#include "GameStatistics.h"
#include "BeastGameInstance.generated.h"

// USTRUCT(BlueprintType)
// struct FBeastSessionResult
// {
// 	GENERATED_USTRUCT_BODY()
//
// 	FOnlineSessionSearchResult OnlineResult;
//
// 	UPROPERTY(BlueprintReadOnly)
// 	FString SessionName;
// };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsCompleted, const TArray<FBlueprintSessionResult>&, SessionResults, bool, Successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FBeastOnDestroySessionComplete, bool Successful);

UCLASS()
class BEASTINTHELABYRINTH_API UBeastGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool HostGame();

	UFUNCTION(BlueprintCallable)
	bool FindGames(const bool bFindLan);

	UFUNCTION(BlueprintCallable)
	void JoinGame(const FBlueprintSessionResult & Session);
	
	UFUNCTION(BlueprintCallable)
	void QuitToMenu();
	
	UFUNCTION(BlueprintCallable)
	FString GetSessionState() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnFindSessionsCompleted OnSessionsFound;

	FBeastOnDestroySessionComplete OnDestroySessionCompleteEvent;

	UFUNCTION(BlueprintCallable)
	UGameStatistics* GetGameStatistics() const { return GameStatistics; }

	void CreateGameStatistics()	{ GameStatistics = NewObject<UGameStatistics>(); }

	FString GetProfilePlayerName() const { return ProfilePlayerName; }

protected:
	void DestroySession();
	void OnDestroySessionComplete(FName InSessionName, bool WasSuccessful);
	
	UPROPERTY(BlueprintReadWrite)
	FString ProfilePlayerName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UGameStatistics* GameStatistics;

private:
		
	virtual void Init() override;
	
	void OnCreatePresenceSessionComplete(FName SessionName, bool WasSuccessful);
	void OnFindSessionsCompleted(const TArray<FOnlineSessionSearchResult>& SessionResults, const bool bSuccessful);
	void OnJoinCompleted(EOnJoinSessionCompleteResult::Type Result);
	void OnTravelLocalSessionFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& Error);
	
	void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);
	void ClientTravelToCurrentSession() const;
	
	ABeastGameSession* GetGameSession() const;
	
	FString LevelName;
	FString LevelOptions;

	FDelegateHandle CreatePresenceSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	// Error handling
	FDelegateHandle TravelLocalSessionFailureDelegateHandle;
	FDelegateHandle NetworkFailureDelegateHandle;
};
