// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/InventoryToolTip.h"
#include "Items/ItemBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UserInterface/Inventory/DragItemVisual.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/InventoryComponent.h"

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();


	UpdateItemImage();
}

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ItemImage->BrushDelegate.BindUFunction(this, "UpdateBrush");
}


FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply =  Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}	

	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// store mouse position when drag detected, used for calculation drop location in InventoryPanel.cpp
	MousePosWhenDragged = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	//test
	UDragDropOperation* ItemDragOperation = NewObject<UDragDropOperation>();

	ItemDragOperation->Payload = Item;

	ItemDragOperation->DefaultDragVisual = this;
	ItemDragOperation->Pivot = EDragPivot::MouseDown;

	Item->OwningInventory->RemoveItem(Item);

	//removes the itemslot from inventory panel
	RemoveFromParent();

	OutOperation = ItemDragOperation;
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

FSlateBrush UInventoryItemSlot::UpdateBrush()
{
	//TODO: create and return slatebrush with item icon image and item image size
	//FSlateImageBrush brush(Item->ItemAssetData.Icon, ItemSize);

	return FSlateImageBrush(Item->GetIconImage(), ItemSize);
}

void UInventoryItemSlot::RotateItem()
{
	Item->Rotate();
	UpdateItemImage();
}

void UInventoryItemSlot::UpdateItemImage()
{
	if (!Item) return;

	ItemSize = Item->GetDimensions() * TileSize;

	BackgroundSizeBox->SetWidthOverride(ItemSize.X);
	BackgroundSizeBox->SetHeightOverride(ItemSize.Y);

	UCanvasPanelSlot* ItemImageSlot = Cast<UCanvasPanelSlot>(ItemImage->Slot);
	ItemImageSlot->SetSize(ItemSize);
}
