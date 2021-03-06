// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interaction/InteractableComponent.h"
#include "GameFramework/Character.h"
#include "Net/VoiceConfig.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BEASTINTHELABYRINTH_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	class UInventoryComponent* PlayerInventory;

	bool IsInteracting() const;

	float GetRemainingInteractionTime() const;

	UFUNCTION(BlueprintCallable, Category="Items")
	void UseItem(class AItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(class AItem* Item);

	UFUNCTION(BlueprintCallable, Category="Items")
	void DropItem(class AItem* Item);

	UFUNCTION(BlueprintCallable, Category="Items")
	void DropSelectedItem();

	UFUNCTION(BlueprintCallable, Category="Items")
	void DropAllItems();

	UFUNCTION(Server, Reliable)
	void ServerDropItem(class AItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerDropSelectedItem();
	
	UFUNCTION(Server, Reliable)
	void ServerDropAllItems();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundAttenuation* AttenuationSettings;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundEffectSourcePresetChain* SourceEffectChain;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UVOIPTalker* VOIPTalker;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionCheckDistance;

	UPROPERTY()
	FInteractionData InteractionData;
	
	FORCEINLINE UInteractableComponent* GetInteractable() const
	{
		return InteractionData.ViewedInteractionComponent;
	}

	void PerformInteractionCheck();
	void CouldNotFindInteractable();
	void FoundNewInteractable(UInteractableComponent* Interactable);
	

	UFUNCTION(Server, Reliable)
	void ServerBeginInteract();
	UFUNCTION(Server, Reliable)
	void ServerEndInteract();

	UFUNCTION(BlueprintCallable)
	void BeginInteract();
	
	void Interact();

	UFUNCTION(BlueprintCallable)
	void EndInteract();

	void InitVOIP();
	void VOIPPostInit();
	

	FTimerHandle TimerHandle_Interact;
	FTimerHandle TimerHandle_VOIPInit;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
