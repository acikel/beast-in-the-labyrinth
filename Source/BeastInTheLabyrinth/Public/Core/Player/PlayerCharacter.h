// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interaction/InteractableComponent.h"
#include "GameFramework/Character.h"
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
	void UseItem(class UItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(class UItem* Item);

	UFUNCTION(BlueprintCallable, Category="Items")
	void DropItem(class UItem* Item);

	UFUNCTION(BlueprintCallable, Category="Items")
	void DropSelectedItem();

	UFUNCTION(Server, Reliable)
	void ServerDropItem(class UItem* Item);

	UFUNCTION(Server, Reliable)
	void ServerDropSelectedItem();

	UPROPERTY(EditDefaultsOnly, Category="Items")
	TSubclassOf<class APickup> PickupClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionCheckDistance;
	
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
	

	FTimerHandle TimerHandle_Interact;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
