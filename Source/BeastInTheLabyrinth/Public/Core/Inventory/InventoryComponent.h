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
	bool AddItem(class UItem* Item);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(class UItem* Item);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveSelectedItem();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	UItem* FindItem(class UItem* Item) const;

	UFUNCTION(Client, Reliable)
	void ClientRefreshInventory();

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 InventorySize = 3;

	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE TArray<class UItem*> GetInventoryItems() const { return InventoryItems; }

	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE class UItem* GetSelectedItem() const { return (SelectedInventoryIndex < InventoryItems.Num() && SelectedInventoryIndex >= 0) ? InventoryItems[SelectedInventoryIndex] : nullptr; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Items, VisibleAnywhere, Category="Inventory")
	TArray<class UItem*> InventoryItems;

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	int32 SelectedInventoryIndex;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual void OnComponentCreated() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UFUNCTION()
	void OnRep_Items();

	UPROPERTY()
	int32 ReplicatedItemsKey;
		
};
