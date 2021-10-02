// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/InventoryComponent.h"

#include "Core/Inventory/Item.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::NextItem()
{
	++SelectedInventoryIndex;
	if (SelectedInventoryIndex >= InventoryItems.Num())
		SelectedInventoryIndex = 0;
	
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::PreviousItem()
{
	--SelectedInventoryIndex;
	if (SelectedInventoryIndex < 0)
		SelectedInventoryIndex = InventoryItems.Num() - 1;
	
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SelectItem(int32 ItemIndex)
{
	if (ItemIndex < 0 || ItemIndex >= InventoryItems.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::SelectItem - Index out of bounds"))
		return;
	}

	SelectedInventoryIndex = ItemIndex;
}

bool UInventoryComponent::AddItem(UItem* Item)
{
	int32 emptyIndex = -1;
	if (InventoryItems.Num() > 0)
	{
		for (int32 i = 0; i < InventoryItems.Num(); ++i)
		{
			if (!InventoryItems[i])
			{
				emptyIndex = i;
				break;
			}
		}
	}
	if (emptyIndex < 0 && InventoryItems.Num() < InventorySize)
	{
		emptyIndex = InventoryItems.Num();
		InventoryItems.Add(nullptr);
	}

	if (emptyIndex != -1)
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			UItem* NewItem = NewObject<UItem>(GetOwner(), Item->GetClass());
			NewItem->OwningInventory = this;
			NewItem->AddedToInventory(this);
			
			InventoryItems[emptyIndex] = NewItem;
			NewItem->MarkDirtyForReplication();

			return true;
		}
	}

	return false;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			InventoryItems.RemoveSingle(Item);
			InventoryItems.Add(nullptr);
			ReplicatedItemsKey++;

			return true;
		}
	}

	return false;
}

bool UInventoryComponent::RemoveSelectedItem()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (SelectedInventoryIndex >= 0 && SelectedInventoryIndex < InventoryItems.Num())
		{
			InventoryItems[SelectedInventoryIndex] = nullptr;
			ReplicatedItemsKey++;

			return true;
		}
	}

	return false;
}


UItem* UInventoryComponent::FindItem(UItem* Item) const
{
	if (Item && InventoryItems.Num() > 0)
	{
		for (auto& InventoryItem : InventoryItems)
		{
			if (InventoryItem && InventoryItem->GetClass() == Item->GetClass())
			{
				return InventoryItem;
			}
		}
	}
	return nullptr;
}

void UInventoryComponent::ClientRefreshInventory_Implementation()
{
	OnInventoryUpdated.Broadcast();
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < InventorySize; ++i)
	{
		InventoryItems.Add(nullptr);
	}
}


void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryItems);
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		for (auto& Item : InventoryItems)
		{
			if (Item)
			{
				if (Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->RepKey))
				{
					WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
				}
			}
		}
	}

	return WroteSomething;
}

void UInventoryComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	//SetIsReplicated(true);
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();
}

