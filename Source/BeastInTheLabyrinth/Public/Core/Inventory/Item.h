// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API AItem : public AActor
{
	GENERATED_BODY()

	AItem();
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* ItemIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* Mesh;
	

	UPROPERTY(BlueprintReadOnly)
	class UInventoryComponent* OwningInventory;

	virtual void Use(class APlayerCharacter* Character);
	virtual void AddedToInventory(class UInventoryComponent* Inventory);

	UFUNCTION()
	void SetCanBePickedUp(bool CanBePickedUp);

	UFUNCTION(Server, Reliable)
	void ServerSetCanBePickedUp(bool CanBePickedUp);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	class UInteractableComponent* InteractionComponent;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnUse"))
	void ReceiveOnUse(class APlayerCharacter* Character);

	UFUNCTION()
	void OnTakeItem(APlayerCharacter* Taker);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
};

