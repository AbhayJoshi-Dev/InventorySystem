// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/InventoryToolTip.h"
#include "Items/ItemBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UserInterface/Inventory/DragItemVisual.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ToolTipClass)
	{
		UInventoryToolTip* ToolTip = CreateWidget<UInventoryToolTip>(this, ToolTipClass);
		ToolTip->InventorySlotBeingHovered = this;
		SetToolTip(ToolTip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ItemReference) return;

	switch (ItemReference->ItemQuality)
	{
	case EItemQuality::Shoddy:
		ItemBorder->SetBrushColor(FLinearColor::Gray);
		break;
	case EItemQuality::Common:
		ItemBorder->SetBrushColor(FLinearColor::White);
		break;
	case EItemQuality::Quality:
		ItemBorder->SetBrushColor(FLinearColor::Green);
		break;
	case EItemQuality::Masterwork:
		ItemBorder->SetBrushColor(FLinearColor::Blue);
		break;
	case EItemQuality::Grandmaster:
		ItemBorder->SetBrushColor(FLinearColor::Red);
		break;
	default:
		break;
	}

	ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);

	if (ItemReference->ItemNumericData.bIsStacakable)
	{
		ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
	}
	else
	{
		ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	}
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

	if (!DragItemVisualClass) return;

	const TObjectPtr<UDragItemVisual> DragItemVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
	DragItemVisual->ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
	DragItemVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

	ItemReference->ItemNumericData.bIsStacakable ? DragItemVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity))
												 : DragItemVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

	UItemDragDropOperation* ItemDragOperation = NewObject<UItemDragDropOperation>();
	ItemDragOperation->SourceItem = ItemReference;
	ItemDragOperation->SourceInventory = ItemReference->OwningInventory;

	ItemDragOperation->DefaultDragVisual = DragItemVisual;
	ItemDragOperation->Pivot = EDragPivot::TopLeft;

	OutOperation = ItemDragOperation;
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}
