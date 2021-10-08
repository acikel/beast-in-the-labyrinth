// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEASTINTHELABYRINTH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UItem;
	
public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void NextItem();
	
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void PreviousItem();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SelectItem(int32 ItemIndex);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(class AItem* Item);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(class AItem* Item);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveSelectedItem(bool ShouldDropItem = true);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	AItem* FindItem(class AItem* Item) const;

	UFUNCTION(Client, Reliable)
	void ClientRefreshInventory();

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 InventorySize = 3;

	UPROPERTY(BlueprintReadOnly)
	class UItemSocket* ItemSocket;

	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE TArray<class AItem*> GetInventoryItems() const { return InventoryItems; }

	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE class AItem* GetSelectedItem() const { return (SelectedInventoryIndex < InventoryItems.Num() && SelectedInventoryIndex >= 0) ? InventoryItems[SelectedInventoryIndex] : nullptr; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="Inventory")
	TArray<class AItem*> InventoryItems;

	UPROPERTY(ReplicatedUsing=OnRep_SelectedInventoryIndex, BlueprintReadOnly, Category="Inventory")
	int32 SelectedInventoryIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ItemThrowForce = 100000;

	UFUNCTION(Server, Reliable)
	void ServerNextItem();

	UFUNCTION(Server, Reliable)
	void ServerPreviousItem();

	void RemoveItemAtIndex(int32 Index, bool ShouldDropItem = true);

	void DropItem(AItem* Item);

	
	UFUNCTION()
	void OnRep_SelectedInventoryIndex();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnComponentCreated() override;

	void UpdateItemHolding();

	void StoreItem(AItem* Item);
	void HoldItem(AItem* Item);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedItemsKey;

	UPROPERTY()
	int32 LastSelectedItemIndex;
		
};
