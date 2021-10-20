// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/PlayerCharacter.h"

#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/Inventory/Item.h"
#include "Core/Inventory/InventoryComponent.h"
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

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
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
				GetWorldTimerManager().SetTimer(TimerHandle_VOIPInit, this, &APlayerCharacter::VOIPPostInit, 2.0f, false);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("VOIP Initializing"));
	}
	else
	{
		GetWorldTimerManager().SetTimer(TimerHandle_VOIPInit, this, &APlayerCharacter::InitVOIP, 0.1f, false);
	}
}

void APlayerCharacter::VOIPPostInit()
{
	if (IsOwnedBy(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			const IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();
			if (VoiceInterface.IsValid())
			{
				VoiceInterface->RemoveAllRemoteTalkers();
			}

			PC->ToggleSpeaking(true);
			UE_LOG(LogTemp, Warning, TEXT("Local VOIP Initialized"));
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

