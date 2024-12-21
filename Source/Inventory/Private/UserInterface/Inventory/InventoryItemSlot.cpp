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

	if (!Item) return;

	ItemSize = Item->ItemNumericData.Dimensions * TileSize;

	BackgroundSizeBox->SetWidthOverride(ItemSize.X);
	BackgroundSizeBox->SetHeightOverride(ItemSize.Y);

	UCanvasPanelSlot* ItemImageSlot = Cast<UCanvasPanelSlot>(ItemImage->Slot);
	ItemImageSlot->SetSize(ItemSize);



	//if (!Item) return;

	//switch (Item->ItemQuality)
	//{
	//case EItemQuality::Shoddy:
	//	ItemBorder->SetBrushColor(FLinearColor::Gray);
	//	break;
	//case EItemQuality::Common:
	//	ItemBorder->SetBrushColor(FLinearColor::White);
	//	break;
	//case EItemQuality::Quality:
	//	ItemBorder->SetBrushColor(FLinearColor::Green);
	//	break;
	//case EItemQuality::Masterwork:
	//	ItemBorder->SetBrushColor(FLinearColor::Blue);
	//	break;
	//case EItemQuality::Grandmaster:
	//	ItemBorder->SetBrushColor(FLinearColor::Red);
	//	break;
	//default:
	//	break;
	//}

	//ItemIcon->SetBrushFromTexture(Item->ItemAssetData.Icon);

	//if (Item->ItemNumericData.bIsStacakable)
	//{
	//	ItemQuantity->SetText(FText::AsNumber(Item->Quantity));
	//}
	//else
	//{
	//	ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	//}
}

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//if (ToolTipClass)
	//{
	//	UInventoryToolTip* ToolTip = CreateWidget<UInventoryToolTip>(this, ToolTipClass);
	//	ToolTip->InventorySlotBeingHovered = this;
	//	SetToolTip(ToolTip);
	//}

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

	//if (!DragItemVisualClass) return;

	//const TObjectPtr<UDragItemVisual> DragItemVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
	//DragItemVisual->ItemIcon->SetBrushFromTexture(Item->ItemAssetData.Icon);
	//DragItemVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

	//Item->ItemNumericData.bIsStacakable ? DragItemVisual->ItemQuantity->SetText(FText::AsNumber(Item->Quantity))
	//											 : DragItemVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	
	//test
	UDragDropOperation* ItemDragOperation = NewObject< UDragDropOperation>();

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
	//TODO: create and return slatebrush with item icon image nad item image size
	//FSlateImageBrush brush(Item->ItemAssetData.Icon, ItemSize);

	return FSlateImageBrush(Item->ItemAssetData.Icon, ItemSize);
}
