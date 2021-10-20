// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Interaction/InteractableComponent.h"
#include "Core/Player/PlayerCharacter.h"
#include "Core/Widget/Game/InteractionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UInteractableComponent::UInteractableComponent()
{
	SetComponentTickEnabled(false);

	InteractionTime = 0.f;
	InteractionDistance = 200.f;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	AllowMultipleInteractors = true;

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(300, 50);
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true);
	
	SetIsReplicatedByDefault(true);
}

void UInteractableComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}

void UInteractableComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractableComponent, Enabled);
}

void UInteractableComponent::Deactivate()
{
	Super::Deactivate();

	for (int32 i = Interactors.Num()  - 1; i > 0; --i)
	{
		if (APlayerCharacter* Interactor = Interactors[0])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}

	Interactors.Empty();
}

bool UInteractableComponent::CanInteract(class APlayerCharacter* Character) const
{
	const bool PlayerAlreadyInteracting = !AllowMultipleInteractors && Interactors.Num() >= 1;
	return !PlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr && Enabled;
}

void UInteractableComponent::RefreshWidget()
{
	if (!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
}

void UInteractableComponent::BeginFocus(APlayerCharacter* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}

	OnBeginFocus.Broadcast(Character);

	if (Enabled && Character == UGameplayStatics::GetPlayerPawn(GetOwner(), 0))
	{
		SetHiddenInGame(false);
		RefreshWidget();
	}
}

void UInteractableComponent::EndFocus(class APlayerCharacter* Character)
{
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true);
}

void UInteractableComponent::BeginInteract(class APlayerCharacter* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractableComponent::EndInteract(class APlayerCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractableComponent::Interact(class APlayerCharacter* Character)
{
	const bool bCanInteract = CanInteract(Character);
	UE_LOG(LogTemp, Warning, TEXT("Interact: %s"), (bCanInteract ? TEXT("true") : TEXT("false")));
	
	if (bCanInteract)
	{
		OnInteract.Broadcast(Character);
	}
}

float UInteractableComponent::GetInteractionPercentage()
{
	if (Interactors.IsValidIndex(0))
	{
		if (APlayerCharacter* Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractionTime() / InteractionTime);
			}
		}
	}

	return 0.f;
}
