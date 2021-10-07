// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/InventoryComponent.h"

#include "Core/Inventory/Item.h"
#include "Core/Inventory/ItemSocket.h"
#include "Core/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::NextItem()
{
	if (AItem* Item = GetSelectedItem())
	{
		StoreItem(Item);
	}
	
	++SelectedInventoryIndex;
	if (SelectedInventoryIndex >= InventoryItems.Num())
		SelectedInventoryIndex = 0;

	if (AItem* Item = GetSelectedItem())
	{
		HoldItem(Item);
	}
	
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::PreviousItem()
{
	--SelectedInventoryIndex;
	if (SelectedInventoryIndex < 0)
		SelectedInventoryIndex = InventoryItems.Num() - 1;

	UpdateItemHolding();
	
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

bool UInventoryComponent::AddItem(AItem* Item)
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
			Item->SetCanBePickedUp(false);
			
			StoreItem(Item);
			
			InventoryItems[emptyIndex] = Item;
			ReplicatedItemsKey++;
			
			OnInventoryUpdated.Broadcast();
			if (emptyIndex == SelectedInventoryIndex)
			{
				HoldItem(Item);
			}
			
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::RemoveItem(AItem* Item)
{

	return false;
}

bool UInventoryComponent::RemoveSelectedItem()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (SelectedInventoryIndex >= 0 && SelectedInventoryIndex < InventoryItems.Num())
		{
			AItem* Item = InventoryItems[SelectedInventoryIndex];
			
			InventoryItems[SelectedInventoryIndex] = nullptr;
			ReplicatedItemsKey++;

			FDetachmentTransformRules DetachmentTransformRules(
				EDetachmentRule::KeepWorld,
				true
				);
			
			Item->DetachFromActor(DetachmentTransformRules);
			Item->SetHidden(false);

			Item->Mesh->SetSimulatePhysics(true);
			Item->SetCanBePickedUp(true);

			FVector Forward;

			if (ItemSocket)
				Forward = ItemSocket->GetForwardVector();
			else
				Forward = GetOwner()->GetActorForwardVector();

			const FVector CalculatedThrowForce = Forward * ItemThrowForce;
			
			Item->Mesh->AddForce(CalculatedThrowForce);

			return true;
		}
	}

	return false;
}


AItem* UInventoryComponent::FindItem(AItem* Item) const
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

	if (GetOwner())
	{
		ItemSocket = static_cast<UItemSocket*>(GetOwner()->GetComponentByClass(UItemSocket::StaticClass()));
	}
	
	for (int32 i = 0; i < InventorySize; ++i)
	{
		InventoryItems.Add(nullptr);
	}
}


void UInventoryComponent::OnRep_SelectedInventoryIndex()
{
	UpdateItemHolding();
	
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryItems);
	DOREPLIFETIME(UInventoryComponent, SelectedInventoryIndex);
}


void UInventoryComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	//SetIsReplicated(true);
}

void UInventoryComponent::UpdateItemHolding()
{
	if (ItemSocket)
	{
		if (LastSelectedItemIndex >= 0 && LastSelectedItemIndex < InventoryItems.Num())
		{
			AItem* LastItem = InventoryItems[LastSelectedItemIndex];
			if (LastItem)
			{
				StoreItem(LastItem);
			}
		}

		AItem* Item = GetSelectedItem();
		if (Item)
		{
			HoldItem(Item);
		}
	
		LastSelectedItemIndex = SelectedInventoryIndex;
	}
}

void UInventoryComponent::StoreItem(AItem* Item)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Item->DisableComponentsSimulatePhysics();
		
		FAttachmentTransformRules AttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false);
		
		Item->AttachToActor(GetOwner(), AttachmentTransformRules);
		Item->SetActorLocation(GetOwner()->GetActorLocation());

		Item->SetHidden(true);
	}
}

void UInventoryComponent::HoldItem(AItem* Item)
{
	if (!ItemSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("No item socket was set. Cannot hold Item"))
		return;
	}
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FAttachmentTransformRules AttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false);

		Item->AttachToComponent(ItemSocket, AttachmentTransformRules);
		Item->SetHidden(false);
	}
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

