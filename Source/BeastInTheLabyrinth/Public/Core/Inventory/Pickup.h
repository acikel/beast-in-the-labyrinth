// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UItem* ItemTemplate;

	void InitializePickup(const TSubclassOf<UItem> ItemClass);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing=OnRep_Item)
	UItem* Item;

	UFUNCTION()
	void OnRep_Item();

	UFUNCTION()
	void OnTakePickup(class APlayerCharacter* Taker);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInteractableComponent* InteractionComponent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
