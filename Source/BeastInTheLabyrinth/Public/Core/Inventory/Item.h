// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseItem, class APlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropItem, class APlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakeItem, class APlayerCharacter*, Character);

/**
 * 
 */
UCLASS(Blueprintable)
class BEASTINTHELABYRINTH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* ItemIcon;
	
	UPROPERTY(BlueprintReadOnly)
	class UInventoryComponent* OwningInventory;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	class UPrimitiveComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector HoldPositionOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator HoldRotationOffset;

	virtual void Use(class APlayerCharacter* Character);
	virtual void AddedToInventory(class UInventoryComponent* Inventory);

	UFUNCTION()
	void SetCanBePickedUp(bool CanBePickedUp);

	UFUNCTION(Server, Reliable)
	void ServerSetCanBePickedUp(bool CanBePickedUp);

	void OnDropItem(class APlayerCharacter* Character);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	class UInteractableComponent* InteractionComponent;

	UPROPERTY(BlueprintAssignable)
	FOnUseItem OnUse;

	UPROPERTY(BlueprintAssignable)
	FOnDropItem OnDrop;

	UPROPERTY(BlueprintAssignable)
	FOnTakeItem OnTake;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnUse"))
	void ReceiveOnUse(class APlayerCharacter* Character);

	UFUNCTION()
	void OnTakeItem(APlayerCharacter* Taker);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
};

