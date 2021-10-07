// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BeastGameSession.h"
#include "FindSessionsCallbackProxy.h"
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

UCLASS()
class BEASTINTHELABYRINTH_API UBeastGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool HostSession();

	UFUNCTION(BlueprintCallable)
	bool FindSessions(const bool bFindLan);

	UFUNCTION(BlueprintCallable)
	void JoinSession(const FBlueprintSessionResult & Session);

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void QuitToMenu();
	
	UFUNCTION(BlueprintCallable)
	FString GetSessionState() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnFindSessionsCompleted OnSessionsFound;

protected:
	UPROPERTY(BlueprintReadWrite)
	FString ProfilePlayerName;

private:
		
	virtual void Init() override;
	
	void OnCreatePresenceSessionComplete(FName SessionName, bool WasSuccessful);
	void OnFindSessionsCompleted(const TArray<FOnlineSessionSearchResult>& SessionResults, const bool bSuccessful);
	void OnJoinCompleted(EOnJoinSessionCompleteResult::Type Result);
	void OnTravelLocalSessionFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
	
	void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);
	void ClientTravelToCurrentSession() const;
	
	ABeastGameSession* GetGameSession() const;
	
	FString LevelName;
	FString LevelOptions;

	FDelegateHandle CreatePresenceSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	// Error handling
	FDelegateHandle TravelLocalSessionFailureDelegateHandle;
};
