// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOperation.generated.h"

class UInventoryItemSlot;

/**
 * 
 */
UCLASS()
class INVENTORY_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	UInventoryItemSlot* ItemSlot;
	
};
