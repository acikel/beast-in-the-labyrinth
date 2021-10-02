// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/Pickup.h"
#include "Core/Interaction/InteractableComponent.h"
#include "Core/Inventory/InventoryComponent.h"
#include "Core/Player/PlayerCharacter.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	SetRootComponent(Mesh);
	
	InteractionComponent = CreateDefaultSubobject<UInteractableComponent>("InteractionComponent");
	InteractionComponent->InteractionTime = 0;
	InteractionComponent->InteractionDistance = 200;
	InteractionComponent->InteractableNameText = FText::FromString("Pickup");
	InteractionComponent->InteractableActionText = FText::FromString("Take");
	InteractionComponent->OnEndInteract.AddDynamic(this, &APickup::OnTakePickup);
	InteractionComponent->SetupAttachment(Mesh);

	bReplicates = true;
}

void APickup::InitializePickup(const TSubclassOf<UItem> ItemClass)
{
	if (HasAuthority() && ItemClass)
	{
		Item = NewObject<UItem>(this, ItemClass);

		OnRep_Item();
		Item->MarkDirtyForReplication();
	}
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && ItemTemplate && bNetStartup)
	{
		InitializePickup(ItemTemplate->GetClass());
	}

	if (!bNetStartup)
	{
		//AlignWithGround();
	}

	if (Item)
	{
		Item->MarkDirtyForReplication();
	}
}

void APickup::OnRep_Item()
{
	if (Item)
	{
		Mesh->SetStaticMesh(Item->Mesh);
		InteractionComponent->InteractableNameText = Item->ItemName;

		InteractionComponent->RefreshWidget();
	}
}

void APickup::OnTakePickup(APlayerCharacter* Taker)
{
	if (!Taker)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pickup was taken but player was not valid."));
		return;
	}

	if (HasAuthority() && !IsPendingKillPending() && Item)
	{
		if (UInventoryComponent* PlayerInventory = Taker->PlayerInventory)
		{
			const bool WasAdded = PlayerInventory->AddItem(Item);

			if (WasAdded)
			{
				UE_LOG(LogTemp, Warning, TEXT("Pickup was taken."));
				Destroy();
			}
		}
	}
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, Item);
}

bool APickup::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (Item && Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->RepKey))
	{
		WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.GetPropertyName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(APickup, ItemTemplate))
	{
		if (ItemTemplate)
		{
			Mesh->SetStaticMesh(ItemTemplate->Mesh);
		}
	}
}
#endif

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

