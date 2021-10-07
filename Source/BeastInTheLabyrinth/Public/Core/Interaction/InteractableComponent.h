// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class APlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class APlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class APlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class APlayerCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class APlayerCharacter*, Character);


USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		InteractionHeld = false;
	}

	UPROPERTY()
	class UInteractableComponent* ViewedInteractionComponent;

	UPROPERTY()
	float LastInteractionCheckTime;

	UPROPERTY()
	bool InteractionHeld;
};


/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BEASTINTHELABYRINTH_API UInteractableComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	FText InteractableNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	FText InteractableActionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction")
	bool AllowMultipleInteractors;

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void SetInteractableActionText(const FText& NewActionText);

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnInteract OnInteract;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
	bool Enabled = true;
	
protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Deactivate() override;

	bool CanInteract(class APlayerCharacter* Character) const;

	UPROPERTY()
	TArray<class APlayerCharacter*> Interactors;

public:

	void RefreshWidget();

	void BeginFocus(class APlayerCharacter* Character);
	void EndFocus(class APlayerCharacter* Character);

	void BeginInteract(class APlayerCharacter* Character);
	void EndInteract(class APlayerCharacter* Character);

	void Interact(class APlayerCharacter* Character);

	UFUNCTION(BlueprintPure, Category="Interaction")
	float GetInteractionPercentage();
	
};
