// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/Item.h"

#include "Core/Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

void UItem::Use(class APlayerCharacter* Character)
{
	ReceiveOnUse(Character);
}

void UItem::AddedToInventory(UInventoryComponent* Inventory)
{
}

void UItem::MarkDirtyForReplication()
{
	++RepKey;

	if (OwningInventory)
	{
		++OwningInventory->ReplicatedItemsKey;
	}
}

void UItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UItem::IsSupportedForNetworking() const
{
	return true;
}
