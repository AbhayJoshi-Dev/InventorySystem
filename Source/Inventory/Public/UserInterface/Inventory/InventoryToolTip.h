// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryToolTip.generated.h"

class UInventoryItemSlot;
class UTextBlock;

UCLASS()
class INVENTORY_API UInventoryToolTip : public UUserWidget
{
	GENERATED_BODY()

public:

	//owning itemSlot
	UPROPERTY(VisibleAnywhere)
	UInventoryItemSlot* InventorySlotBeingHovered;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UsageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemDescription;

protected:

	virtual void NativeConstruct() override;
};
