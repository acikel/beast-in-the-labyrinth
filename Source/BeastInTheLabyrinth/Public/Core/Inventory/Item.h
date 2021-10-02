// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class BEASTINTHELABYRINTH_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText ItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* ItemIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY()
	int32 RepKey;

	class UInventoryComponent* OwningInventory;

	virtual void Use(class APlayerCharacter* Character);
	virtual void AddedToInventory(class UInventoryComponent* Inventory);

	void MarkDirtyForReplication();

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnUse"))
	void ReceiveOnUse(class APlayerCharacter* Character);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
};
