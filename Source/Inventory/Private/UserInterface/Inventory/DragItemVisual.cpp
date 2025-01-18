// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/DragItemVisual.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"


void UDragItemVisual::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ItemIcon->BrushDelegate.BindUFunction(this, "UpdateBrush");
}

void UDragItemVisual::SetImageAndSize(UMaterialInterface* Image, FVector2D Size)
{
	ImageMaterial = Image;
	ImageSize = Size;

	BackgroundSizeBox->SetWidthOverride(ImageSize.X);
	BackgroundSizeBox->SetHeightOverride(ImageSize.Y);

	UCanvasPanelSlot* ItemImageSlot = Cast<UCanvasPanelSlot>(ItemIcon->Slot);
	ItemImageSlot->SetSize(ImageSize);
}

void UDragItemVisual::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// store mouse position when drag detected, used for calculation drop location in InventoryPanel.cpp
	MousePosWhenDragged = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
}

FSlateBrush UDragItemVisual::UpdateBrush()
{
	return FSlateImageBrush(ImageMaterial, ImageSize);
}
