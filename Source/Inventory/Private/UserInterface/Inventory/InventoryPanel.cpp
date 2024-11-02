// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryPanel.h"
#include "Character/PlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/Border.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "Components/CanvasPanelSlot.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

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
}

void UInventoryPanel::CreateLineSegments()
{
	// Vertical Lines
	int32 CurrentX = 0;

	for (int i = 0; i <= InventoryComponent->GetColumns(); i++)
	{
		CurrentX = i * TileSize;

		FVector2D StartLine{ (float)CurrentX, 0 };
		FVector2D EndLine{ (float)CurrentX, InventoryComponent->GetRows() * TileSize};
		Lines.Add(FLine(StartLine, EndLine));
	}

	// Horizontal Lines
	int32 CurrentY = 0;

	for (int i = 0; i <= InventoryComponent->GetRows(); i++)
	{
		CurrentY = i * TileSize;

		FVector2D StartLine{ 0, (float)CurrentY };
		FVector2D EndLine{ InventoryComponent->GetColumns() * TileSize, (float)CurrentY };
		Lines.Add(FLine(StartLine, EndLine));
	}
}

void UInventoryPanel::RefreshInventory()
{
	
}

//void UInventoryPanel::RefreshInventory()
//{
//	if (!InventoryComponent || !InventorySlotClass) return;
//
//	InventoryWrapBox->ClearChildren();
//
//	for (UItemBase* const& InventoryItem : InventoryComponent->GetInventoryContents())
//	{
//		UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
//		ItemSlot->SetItemReference(InventoryItem);
//			
//		InventoryWrapBox->AddChildToWrapBox(ItemSlot);
//	}
//
//	SetInfoText();
//}

//void UInventoryPanel::SetInfoText() const
//{
//	const FString WeightInfoValue = { FString::SanitizeFloat(InventoryComponent->GetInventoryTotalWeight()) + "/"
//										+ FString::SanitizeFloat(InventoryComponent->GetWeightCapacity()) };
//
//	WeightInfo->SetText(FText::FromString(WeightInfoValue));
//
//	const FString CapacityInfoValue = { FString::FromInt(InventoryComponent->GetInventoryContents().Num()) + "/"
//										+ FString::FromInt(InventoryComponent->GetSlotsCapacity()) };
//
//	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
//}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on Inventory Panel."));

		return true;
	}

	return false;
}

int32 UInventoryPanel::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	for (int i = 0; i < Lines.Num(); i++)
	{
		FVector2D TopLeftposition{ 0, 0 };
		UWidgetBlueprintLibrary::DrawLine(Context, TopLeftposition + Lines[i].Start, TopLeftposition + Lines[i].End, FLinearColor(FVector4d(0.5f, 0.5f, 0.5f, 0.5f)));
	}

	return LayerId + 1;
}

