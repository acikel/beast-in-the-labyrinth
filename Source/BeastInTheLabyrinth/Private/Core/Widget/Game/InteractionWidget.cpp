// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Widget/Game/InteractionWidget.h"

void UInteractionWidget::UpdateInteractionWidget(UInteractableComponent* InteractableComponent)
{
	OwningInteractionComponent = InteractableComponent;
	OnUpdateInteractionWidget();
}
