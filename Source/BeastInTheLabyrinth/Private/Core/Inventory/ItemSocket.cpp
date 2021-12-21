// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Inventory/ItemSocket.h"


// Sets default values for this component's properties
UItemSocket::UItemSocket()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UItemSocket::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UItemSocket::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemSocket::HoldItem(AItem* Item)
{
	FAttachmentTransformRules AttachmentTransformRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false);

	Item->DisableComponentsSimulatePhysics();
	Item->AttachToComponent(this, AttachmentTransformRules);
	Item->SetCanBePickedUp(false);
	//Item->SetActorHiddenInGame(false);
	HoldingItem = Item;
}

void UItemSocket::ReleaseItem(AItem* Item)
{
	const FDetachmentTransformRules DetachmentTransformRules(
		EDetachmentRule::KeepWorld,true
	);
	
	Item->DetachFromActor(DetachmentTransformRules);
	Item->SetCanBePickedUp(true);
	
	HoldingItem = nullptr;
}

