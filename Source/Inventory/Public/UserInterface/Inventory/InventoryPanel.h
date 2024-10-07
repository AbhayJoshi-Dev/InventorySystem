// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.h"
#include "InventoryPanel.generated.h"

class UWrapBox;
class UTextBlock;
class APlayerCharacter;
class UInventoryComponent;

UCLASS()
class INVENTORY_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryWrapBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

protected:

	void SetInfoText() const;
	void NativeOnInitialized() override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
