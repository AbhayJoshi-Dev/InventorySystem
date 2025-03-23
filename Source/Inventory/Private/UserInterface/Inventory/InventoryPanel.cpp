// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryPanel.h"
#include "Character/PlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/Border.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Items/ItemBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Slate/SlateBrushAsset.h"

#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Kismet/GameplayStatics.h"

#include "UserInterface/Inventory/DragItemVisual.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// TODO: if anything else than player can have a inventory, improve how we get pointer to inventory component

	PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());

	if (PlayerCharacter)
	{
		InventoryComponent = PlayerCharacter->GetInventory();
		if (!InventoryComponent) return;

		InventoryComponent->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
		//SetInfoText();
	}


	GridBorderSlot = Cast<UCanvasPanelSlot>(GridBorder->Slot);
	GridBorderSlot->SetSize(FVector2D(TileSize * InventoryComponent->GetColumns(), TileSize * InventoryComponent->GetRows()));

	CreateLineSegments();

	SlateBrushAsset = NewObject<USlateBrushAsset>();
}

void UInventoryPanel::CreateLineSegments()
{
	// Vertical Lines
	int32 CurrentX = 0;

	for (int i = 0; i <= InventoryComponent->GetColumns(); i++)
	{
		CurrentX = i * TileSize;

		FVector2D StartLine{ (float)CurrentX, 0 };
		FVector2D EndLine{ (float)CurrentX, InventoryComponent->GetRows() * (float)TileSize};
		Lines.Add(FInventoryLine(StartLine, EndLine));
	}

	// Horizontal Lines
	int32 CurrentY = 0;

	for (int i = 0; i <= InventoryComponent->GetRows(); i++)
	{
		CurrentY = i * TileSize;

		FVector2D StartLine{ 0, (float)CurrentY };
		FVector2D EndLine{ InventoryComponent->GetColumns() * (float)TileSize, (float)CurrentY };
		Lines.Add(FInventoryLine(StartLine, EndLine));
	}
}

void UInventoryPanel::RefreshInventory()
{
	if (!InventoryComponent) return;

	GridCanvasPanel->ClearChildren();

	TMap<UItemBase*, FTile> ItemsMap = InventoryComponent->GetUniqueItems();

	for (const TPair<UItemBase*, FTile>& Pair : ItemsMap)
	{
		UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);

		ItemSlot->SetItemReference(Pair.Key);
		ItemSlot->SetTileSize(TileSize);

		//TODO: Bind on removed event / dispatcher

		UPanelSlot* PanelSlot = GridCanvasPanel->AddChild(ItemSlot);

		UCanvasPanelSlot* GridCanvasPanelSlot = Cast<UCanvasPanelSlot>(PanelSlot);
		GridCanvasPanelSlot->SetAutoSize(true);

		GridCanvasPanelSlot->SetPosition(FVector2D(Pair.Value.X * TileSize, Pair.Value.Y * TileSize));
	}
}

//void UInventoryPanel::SetInfoText() const
//{
//	const FString WeightInfoValue = { FString::SanitizeFloat(InventoryComponent->GetInventoryTotalWeight()) + "/"
//										+ FString::SanitizeFloat(InventoryComponent->GetWeightCapacity()) };
//
//	WeightInfo->SetText(FText::FromString(WeightInfoValue));
//}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	DrawDropLocation = false;

	if (InOperation->Payload && InventoryComponent)
	{
		// Check if room available for payload, get the top left index from on drag over override function
		UItemBase* Item = Cast<UItemBase>(InOperation->Payload);

		if (InventoryComponent->IsRoomAvailable(Item, DraggedTopLeftIndex))
		{
			InventoryComponent->AddNewItem(Item, DraggedTopLeftIndex);
		}
		// try add item in any other place in inventory because DraggedTopLeftIndex already has something
		else
		{
			// if no room available , drop on the ground
			if (!InventoryComponent->HandleAddItem(Item))
			{
				Item->OwningInventory->DropItem(Item);
			}
		}

		return true;
	}

	return false;
}


bool UInventoryPanel::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
//UDragItemVisual* DragItemVisual = Cast<UDragItemVisual>(InOperation->DefaultDragVisual);
//	FVector2D MousePositionOnItemSlot = DragItemVisual->GetMousePosWhenDragged();

	MousePositionOnGrid = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	
	FVector2D TopLeftPosition(MousePositionOnGrid.X, MousePositionOnGrid.Y);
		
	// clamping top left pos according to item dimensions, to prevent drop location drawing outside
	FVector2D ItemDimensions = Cast<UItemBase>(InOperation->Payload)->GetDimensions();
	TopLeftPosition.X = FMath::Clamp(TopLeftPosition.X, 0.f, (InventoryComponent->GetRows() - ItemDimensions.X) * TileSize);
	TopLeftPosition.Y = FMath::Clamp(TopLeftPosition.Y, 0.f, (InventoryComponent->GetColumns() - ItemDimensions.Y) * TileSize);

	int32 X = (int32)TopLeftPosition.X % TileSize;
	int32 Y = (int32)TopLeftPosition.Y % TileSize;

	FIntPoint Mod(X, Y);
	
	FIntPoint P(0, 0);

	P.X = (Mod.X > TileSize / 2) ? 1 : 0;
	P.Y = (Mod.Y > TileSize / 2) ? 1 : 0;
	
	// TODO: currently calculated DraggedTopLeftIndex will be zero
	FIntPoint TopLeftTile(TopLeftPosition.X / TileSize + P.X, TopLeftPosition.Y / TileSize + P.Y);
	DraggedTopLeftIndex = InventoryComponent->TileToIndex({ TopLeftTile.X, TopLeftTile.Y });


	return true;
}


int32 UInventoryPanel::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// draw inventory grid lines
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	//TODO: remove Lines array and calculate lines pos here

	for (int i = 0; i < Lines.Num(); i++)
	{
		UWidgetBlueprintLibrary::DrawLine(Context, Lines[i].Start, Lines[i].End, FLinearColor(0.5f, 0.5f, 0.5f, 0.5f));
	}


	// draw item drag drop location
	if (UWidgetBlueprintLibrary::IsDragDropping() && DrawDropLocation)
	{
		UItemBase* Item = Cast<UItemBase>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);

		FLinearColor Tint;

		if (InventoryComponent->IsRoomAvailable(Item, DraggedTopLeftIndex))
		{
			Tint = { 0.f, 1.f, 0.f , 0.25f };
		}
		else
		{
			Tint = { 1.f, 0.f, 0.f , 0.25f };
		}

		FTile PosTile = InventoryComponent->IndexToTile(DraggedTopLeftIndex);
		FVector2D Pos(PosTile.X * TileSize, PosTile.Y * TileSize);

		Pos.X = FMath::Clamp(Pos.X, 0.f, InventoryComponent->GetRows() * TileSize);
		Pos.Y = FMath::Clamp(Pos.Y, 0.f, InventoryComponent->GetColumns() * TileSize);

		FVector2D Size(Item->GetDimensions().X * TileSize, Item->GetDimensions().Y * TileSize);
		UWidgetBlueprintLibrary::SetBrushResourceToMaterial(SlateBrushAsset->Brush, nullptr);
		UWidgetBlueprintLibrary::DrawBox(Context,
			Pos, 
			Size,
			SlateBrushAsset,
			Tint);
	}

	FVector2D ClampedPosition = MousePositionOnGrid;
	if (UWidgetBlueprintLibrary::IsDragDropping() && DrawDropLocation)
	{
		// clamping top left pos according to item dimensions, to prevent drop location drawing outside
		FVector2D ItemDimensions = Cast<UItemBase>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload)->GetDimensions();
		ClampedPosition.X = FMath::Clamp(ClampedPosition.X, 0.f, (InventoryComponent->GetRows() - ItemDimensions.X) * TileSize);
		ClampedPosition.Y = FMath::Clamp(ClampedPosition.Y, 0.f, (InventoryComponent->GetColumns() - ItemDimensions.Y) * TileSize);
	}


	//draw item image tile box
	
	FTile Tile(ClampedPosition.X / TileSize, ClampedPosition.Y / TileSize);
	FVector2D Size(TileSize, TileSize);
	FLinearColor Tint(1.f, 1.f, 1.f, 1.f);



	FIntPoint Mod((int32)ClampedPosition.X % TileSize, (int32)ClampedPosition.Y % TileSize);

	FIntPoint P(0, 0);

	P.X = (Mod.X > TileSize / 2) ? 1 : 0;
	P.Y = (Mod.Y > TileSize / 2) ? 1 : 0;

	// draw image if dragging
	if (UWidgetBlueprintLibrary::IsDragDropping() && DrawDropLocation)
	{
		UItemBase* Item = Cast<UItemBase>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);


		// test draw item icon image
		UWidgetBlueprintLibrary::SetBrushResourceToMaterial(SlateBrushAsset->Brush, Item->GetIconImage());
		UWidgetBlueprintLibrary::DrawBox(Context, FVector2D((Tile.X + P.X) * TileSize, (Tile.Y + P.Y) * TileSize), Item->GetDimensions() * TileSize, SlateBrushAsset, Tint);

	}
	// draw transparent white tile if not dragging
	else
	{
		if (!InventoryComponent) return LayerId + 1;

		if (UItemBase* ItemAtMouseTile = InventoryComponent->GetItemAtTile(Tile))
		{
			Tile = InventoryComponent->GetTopLeftTileOfItem(ItemAtMouseTile);
			Size *= ItemAtMouseTile->GetDimensions();
		}

		// draw white mouse follow tile
		UWidgetBlueprintLibrary::SetBrushResourceToMaterial(SlateBrushAsset->Brush, nullptr);
		UWidgetBlueprintLibrary::DrawBox(Context, FVector2D(Tile.X * TileSize, Tile.Y * TileSize), Size, SlateBrushAsset, FLinearColor(1.f, 1.f, 1.f, 0.25f));
	}

	
	return LayerId + 1;
}


void UInventoryPanel::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	DrawDropLocation = true;
}

void UInventoryPanel::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	DrawDropLocation = false;
}

UE_DISABLE_OPTIMIZATION

FReply UInventoryPanel::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::R)
	{
		if (!UWidgetBlueprintLibrary::GetDragDroppingContent()) return Reply.Unhandled();

		UDragDropOperation* DragDropOperation = UWidgetBlueprintLibrary::GetDragDroppingContent();

		if (!DragDropOperation) return Reply.Unhandled();


		if (UItemDragDropOperation* ItemDragDropOperation = Cast<UItemDragDropOperation>(DragDropOperation))
		{
			if (UInventoryItemSlot* InventoryItemSlot = Cast<UInventoryItemSlot>(ItemDragDropOperation->ItemSlot))
			{
				InventoryItemSlot->RotateItem();
				return Reply.Handled();
			}
		}

	}
	return Reply.Unhandled();
}

UE_ENABLE_OPTIMIZATION

FReply UInventoryPanel::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	MousePositionOnGrid = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	return Reply;
}
