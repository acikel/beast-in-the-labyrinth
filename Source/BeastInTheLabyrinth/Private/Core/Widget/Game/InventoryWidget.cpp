// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Widget/Game/InventoryWidget.h"

void UInventoryWidget::UpdateInteractionWidget(UInventoryComponent* InventoryComponent)
{
	OwningInventoryComponent = InventoryComponent;
	OnUpdateInventoryWidget();
}

