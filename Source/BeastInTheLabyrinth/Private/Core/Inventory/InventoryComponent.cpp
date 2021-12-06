// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/InventoryComponent.h"

#include "Kismet/KismetMathLibrary.h"
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
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerNextItem();
	}
	
	++SelectedInventoryIndex;
	if (SelectedInventoryIndex >= InventoryItems.Num())
		SelectedInventoryIndex = 0;
	
	OnInventoryUpdated.Broadcast();
	UpdateItemHolding();
}

void UInventoryComponent::PreviousItem()
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerPreviousItem();
	}
	
	--SelectedInventoryIndex;
	if (SelectedInventoryIndex < 0)
		SelectedInventoryIndex = InventoryItems.Num() - 1;
	
	
	OnInventoryUpdated.Broadcast();
	UpdateItemHolding();
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
	int32 Index = -1;
	InventoryItems.Find(Item, Index);
	if (InventoryItems.IsValidIndex(Index))
	{
		RemoveItemAtIndex(Index);
	}
	
	return false;
}

bool UInventoryComponent::RemoveSelectedItem(bool ShouldDropItem)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (InventoryItems.IsValidIndex(SelectedInventoryIndex))
		{
			AItem* Item = InventoryItems[SelectedInventoryIndex];
			if (Item) {
				InventoryItems[SelectedInventoryIndex] = nullptr;
				ReplicatedItemsKey++;
	
				if (ShouldDropItem)
					DropItem(Item);
				
				OnInventoryUpdated.Broadcast();
	
				return true;
			}
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
		Character = Cast<APlayerCharacter>(GetOwner());
	}
	
	for (int32 i = 0; i < InventorySize; ++i)
	{
		InventoryItems.Add(nullptr);
	}
}


void UInventoryComponent::ServerNextItem_Implementation()
{
	NextItem();
}

void UInventoryComponent::ServerPreviousItem_Implementation()
{
	PreviousItem();
}

void UInventoryComponent::RemoveItemAtIndex(int32 Index, bool ShouldDropItem)
{
	AItem* Item = InventoryItems[Index];
			
	InventoryItems[Index] = nullptr;
	ReplicatedItemsKey++;

	if (ShouldDropItem)
		DropItem(Item);
	
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::DropItem(AItem* Item)
{
	FDetachmentTransformRules DetachmentTransformRules(
	EDetachmentRule::KeepWorld,
	true
	);

	FVector Forward;
	float Pitch = 90.0f;
	float Force;

	if(Character != nullptr)
	{
		Pitch = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(90, 0, 0), Character->GetControlRotation()).Pitch;
	}

	if(Pitch > ThrowForcePeakAtRotation)
	{
		Force = FMath::GetMappedRangeValueClamped(
			FVector2D(ThrowForcePeakAtRotation, ThrowForceMinimumAtRotation),
			FVector2D(ItemThrowForce.GetUpperBoundValue(), ItemThrowForce.GetLowerBoundValue()), Pitch);
	}
	else
	{
		const float MidForce = (ItemThrowForce.GetLowerBoundValue() + ItemThrowForce.GetUpperBoundValue()) / 2;
		Force = FMath::GetMappedRangeValueClamped(
			FVector2D(0, ThrowForcePeakAtRotation),
			FVector2D(MidForce, ItemThrowForce.GetUpperBoundValue()), Pitch);
	}
	
	if (ItemSocket)
		Forward = ItemSocket->GetForwardVector();
	else
		Forward = GetOwner()->GetActorForwardVector();

	const FVector CalculatedThrowForce = Forward * Force;
	

	Item->DetachFromActor(DetachmentTransformRules);
	Item->SetHidden(false);
	Item->SetCanBePickedUp(true);
	Item->OnDropItem(Character);

	Item->Mesh->SetSimulatePhysics(true);
	Item->Mesh->AddForce(CalculatedThrowForce);
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

		Item->SetActorHiddenInGame(true);
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
		Item->SetActorRelativeLocation(Item->HoldPositionOffset);
		Item->SetActorRelativeRotation(Item->HoldRotationOffset);
		Item->SetActorHiddenInGame(false);
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

