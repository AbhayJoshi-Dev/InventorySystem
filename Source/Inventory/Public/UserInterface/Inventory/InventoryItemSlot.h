// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.generated.h"

class UInventoryToolTip;
class UDragItemVisual;
class UImage;
class UItemBase;
class USizeBox;

UCLASS()
class INVENTORY_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FORCEINLINE void SetItemReference(UItemBase* InItem) { Item = InItem; }
	FORCEINLINE UItemBase* GetItemReference() const { return Item; }

	FORCEINLINE void SetTileSize(float Size) { TileSize = Size; }

	UFUNCTION()
	FSlateBrush UpdateBrush();

	FORCEINLINE FVector2D GetMousePosWhenDragged() const { return MousePosWhenDragged; }

	void RotateItem();


protected:

	void UpdateItemImage();

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slots")
	TSubclassOf<UDragItemVisual> DragItemVisualClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slots")
	TSubclassOf<UInventoryToolTip> ToolTipClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot")
	UItemBase* Item;

	UPROPERTY(meta = (BindWidget))
	USizeBox* BackgroundSizeBox;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

private:

	float TileSize;

	FVector2D ItemSize;

	FVector2D MousePosWhenDragged;
};
