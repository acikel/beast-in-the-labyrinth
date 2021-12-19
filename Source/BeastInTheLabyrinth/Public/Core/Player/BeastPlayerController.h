// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BeastPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeName, FString, NewName);

/**
 * 
 */
UCLASS()
class BEASTINTHELABYRINTH_API ABeastPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangePlayerName(const FString& NewPlayerName);

private:
	void RegisterStatistics();
	void SetPlayerName();

	FTimerHandle RegisterStatisticsDelayHandle;
	FTimerHandle SetPlayerNameHandle;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnChangeName OnChangeName;
};
