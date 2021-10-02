// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UpdateInteractionWidget(class UInventoryComponent* InventoryComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateInventoryWidget();

	UPROPERTY(BlueprintReadOnly, Category="Inventory", meta=(ExposeOnSpawn))
	class UInventoryComponent* OwningInventoryComponent;

};
