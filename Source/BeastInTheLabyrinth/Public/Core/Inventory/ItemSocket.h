// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Core/Inventory/Item.h"
#include "ItemSocket.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEASTINTHELABYRINTH_API UItemSocket : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemSocket();

	UFUNCTION(BlueprintCallable, Category="ItemSocket")
	void HoldItem(AItem* Item);

	UFUNCTION(BlueprintCallable, Category="ItemSocket")
	void ReleaseItem(AItem* Item);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AItem* HoldingItem;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
