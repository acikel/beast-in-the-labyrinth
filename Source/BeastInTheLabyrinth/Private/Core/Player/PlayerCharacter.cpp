// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/PlayerCharacter.h"

#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/Inventory/Item.h"
#include "Core/Inventory/InventoryComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionCheckFrequency = 0.f;
	InteractionCheckDistance = 1000.0f;

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>("PlayerInventory");
	PlayerInventory->InventorySize = 3;
	
	bReplicates = true;
}

bool APlayerCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float APlayerCharacter::GetRemainingInteractionTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void APlayerCharacter::ServerDropAllItems_Implementation()
{
	DropAllItems();
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitVOIP();
}

void APlayerCharacter::PerformInteractionCheck()
{
	if (GetController() == nullptr)
	{
		return;
	}

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLocation;
	FRotator EyesRotation;

	GetController()->GetPlayerViewPoint(EyesLocation, EyesRotation);

	FVector TraceStart = EyesLocation;
	FVector TraceEnd = (EyesRotation.Vector() * InteractionCheckDistance) + TraceStart;
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	TArray<AItem*> Items = PlayerInventory->GetInventoryItems();
	for (AItem* Item : Items)
	{
		if (Item)
		{
			QueryParams.AddIgnoredActor(Item);
		}
	}

	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(25.0f);
	
	if (GetWorld()->SweepSingleByChannel(TraceHit, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, CollisionSphere, QueryParams))
	{
		if (TraceHit.GetActor())
		{
			if (UInteractableComponent* InteractionComponent = Cast<UInteractableComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractableComponent::StaticClass())))
			{
				float distance = (TraceStart - TraceHit.ImpactPoint).Size();

				if (InteractionComponent != GetInteractable() && distance <= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractable(InteractionComponent);
				}
				else if (distance > InteractionComponent->InteractionDistance && GetInteractable())
				{
					CouldNotFindInteractable();
				}

				return;
			}
		}
	}
	
	CouldNotFindInteractable();
}

void APlayerCharacter::CouldNotFindInteractable()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	if (UInteractableComponent* Interactable = GetInteractable())
	{
		if (IsValid(Interactable))
		{
			Interactable->EndFocus(this);
		}

		if (InteractionData.InteractionHeld)
		{
			EndInteract();
		}

		InteractionData.ViewedInteractionComponent = nullptr;
	}
}

void APlayerCharacter::FoundNewInteractable(UInteractableComponent* Interactable)
{
	EndInteract();

	if (UInteractableComponent* OldInteractable = GetInteractable())
	{
		if (IsValid(OldInteractable))
		{
			OldInteractable->EndFocus(this);
		}
	}

	if (IsValid(Interactable))
	{
		InteractionData.ViewedInteractionComponent = Interactable;
		Interactable->BeginFocus(this);
	}
}

void APlayerCharacter::BeginInteract()
{
	if (!HasAuthority())
	{
		ServerBeginInteract();
	}

	if (HasAuthority())
	{
		PerformInteractionCheck();
	}

	InteractionData.InteractionHeld = true;

	if (UInteractableComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if (FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &APlayerCharacter::Interact, Interactable->InteractionTime);
		}
	}
}

void APlayerCharacter::EndInteract()
{
	if (!HasAuthority())
	{
		ServerEndInteract();
	}

	InteractionData.InteractionHeld = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractableComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void APlayerCharacter::InitVOIP()
{
	if (APlayerState* PS = GetPlayerState())
	{
		FVoiceSettings VoiceSettings;
		VoiceSettings.ComponentToAttachTo = this->GetRootComponent();
		VoiceSettings.AttenuationSettings = AttenuationSettings;
		VoiceSettings.SourceEffectChain = SourceEffectChain;
		
		VOIPTalker = NewObject<UVOIPTalker>();
		AddInstanceComponent(VOIPTalker);
		
		VOIPTalker->Settings = VoiceSettings;
		VOIPTalker->RegisterWithPlayerState(PS);
				
		UVOIPStatics::SetMicThreshold(-50.0f);

		if (IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->ToggleSpeaking(false);
			}
		}

		GetWorldTimerManager().SetTimer(TimerHandle_VOIPInit, this, &APlayerCharacter::VOIPPostInit, 2.5f, false);

		UE_LOG(LogTemp, Warning, TEXT("VOIP Initializing"));
	}
	else
	{
		GetWorldTimerManager().SetTimer(TimerHandle_VOIPInit, this, &APlayerCharacter::InitVOIP, 0.1f, false);
	}
}

void APlayerCharacter::VOIPPostInit()
{
	const IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();
	if (VoiceInterface.IsValid())
	{
		VoiceInterface->RemoveAllRemoteTalkers();
	}
	
	if (IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->ToggleSpeaking(true);
			UE_LOG(LogTemp, Warning, TEXT("Local VOIP Initialized"));
		}
	}
	else
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->ToggleSpeaking(false);
			PC->ToggleSpeaking(true);
		}
	}
}

void APlayerCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

void APlayerCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

void APlayerCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractableComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

void APlayerCharacter::ServerUseItem_Implementation(AItem* Item)
{
	UseItem(Item);
}

void APlayerCharacter::UseItem(AItem* Item)
{
	if (GetLocalRole() < ROLE_Authority && Item)
	{
		ServerUseItem(Item);
	}

	if (HasAuthority())
	{
		if (PlayerInventory && !PlayerInventory->FindItem(Item))
		{
			return;
		}
	}

	if (Item)
	{
		Item->Use(this);
	}
}

void APlayerCharacter::DropItem(AItem* Item)
{
	if (PlayerInventory && Item && PlayerInventory->FindItem(Item))
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerDropItem(Item);
			return;
		}
	}
	
	if (HasAuthority())
	{
		if (PlayerInventory->RemoveItem(Item))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		
			const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
		}
	}
}

void APlayerCharacter::DropSelectedItem()
{
	if (PlayerInventory)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerDropSelectedItem();
			return;
		}
	}
	
	if (HasAuthority())
	{
		AItem* Item = PlayerInventory->GetSelectedItem();
		if (PlayerInventory->RemoveSelectedItem() && Item)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		
			const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
		}
	}
}

void APlayerCharacter::DropAllItems()
{
	if (PlayerInventory)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerDropAllItems();
			return;
		}
	}

	if (HasAuthority())
	{
		TArray<AItem*> items = PlayerInventory->GetInventoryItems();
		for (int32 i = items.Num() - 1; i >= 0; --i)
		{
			AItem* item = items[i];
			if (item)
			{
				PlayerInventory->RemoveItem(item, true, false);
			}
		}
	}
}

void APlayerCharacter::ServerDropSelectedItem_Implementation()
{
	DropSelectedItem();
}

void APlayerCharacter::ServerDropItem_Implementation(AItem* Item)
{
	DropItem(Item);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool IsInteractingOnServer = (HasAuthority() && IsInteracting());

	if ((IsLocallyControlled() || IsInteractingOnServer) && GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

