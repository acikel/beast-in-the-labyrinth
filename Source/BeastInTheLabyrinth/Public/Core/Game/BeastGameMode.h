// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BeastGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, APlayerController*, NewPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, AController*, ExitingPlayer);

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API ABeastGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable)
	void StartGame(const FString& Url);
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoined;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnPlayerLeft OnPlayerLeft;
};
