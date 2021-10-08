// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/Item.h"

#include "Core/Interaction/InteractableComponent.h"
#include "Core/Inventory/InventoryComponent.h"
#include "Core/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SetRootComponent(Mesh);

	InteractionComponent = CreateDefaultSubobject<UInteractableComponent>("InteractionComponent");
	InteractionComponent->InteractionTime = 0;
	InteractionComponent->InteractionDistance = 250;
	InteractionComponent->InteractableNameText = ItemName;
	InteractionComponent->InteractableActionText = FText::FromString("Take");
	InteractionComponent->OnInteract.AddDynamic(this, &AItem::OnTakeItem);
	InteractionComponent->SetIsReplicated(true);
	InteractionComponent->SetupAttachment(Mesh);
	
	bReplicates = true;
	SetReplicatingMovement(true);
}

void AItem::Use(class APlayerCharacter* Character)
{
	ReceiveOnUse(Character);
	OnUse.Broadcast(Character);
}

void AItem::AddedToInventory(UInventoryComponent* Inventory)
{
}

void AItem::SetCanBePickedUp(bool CanBePickedUp)
{
	if (!HasAuthority())
	{
		ServerSetCanBePickedUp(CanBePickedUp);
	}
	
	InteractionComponent->Enabled = CanBePickedUp;
}

void AItem::ServerSetCanBePickedUp_Implementation(bool CanBePickedUp)
{
	SetCanBePickedUp(CanBePickedUp);
}

void AItem::OnDropItem(APlayerCharacter* Character)
{
	OnDrop.Broadcast(Character);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::Destroyed()
{
	if (OwningInventory)
		OwningInventory->RemoveItem(this);
	
	Super::Destroyed();
}

void AItem::OnTakeItem(APlayerCharacter* Taker)
{
	UE_LOG(LogTemp, Warning, TEXT("Take Item"));
	
	if (!Taker)
	{
		UE_LOG(LogTemp, Error, TEXT("Pickup was taken but player was not valid."));
		return;
	}
	
	if (HasAuthority() && !IsPendingKillPending())
	{
		if (UInventoryComponent* PlayerInventory = Taker->PlayerInventory)
		{
			const bool WasAdded = PlayerInventory->AddItem(this);

			if (WasAdded)
			{
				UE_LOG(LogTemp, Warning, TEXT("Item was taken."));
				OwningInventory = PlayerInventory;
				OnTake.Broadcast(Taker);
			}
		}
	}
}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, InteractionComponent);
}

bool AItem::IsSupportedForNetworking() const
{
	return true;
}
