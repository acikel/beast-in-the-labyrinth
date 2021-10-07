// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BeastPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API ABeastPlayerState : public APlayerState
{
	GENERATED_BODY()


public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	// TODO: Remove later
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void SetNickname(const FString& PlayerName);

	UFUNCTION(BlueprintCallable)
	void SetIsLeader(const bool bIsLeader);

	// TODO: Remove later
	UFUNCTION(BlueprintPure, Category = PlayerState)
	FString GetNickname() const;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsGroupLeader = false;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	FString DisplayName;
	
};
