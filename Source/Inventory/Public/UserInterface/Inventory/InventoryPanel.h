// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.h"
#include "InventoryPanel.generated.h"

class UBorder;
class UCanvasPanel;
class APlayerCharacter;
class UInventoryComponent;
class UCanvasPanelSlot;
class USlateBrushAsset;

USTRUCT(BlueprintType)
struct FInventoryLine
{
	GENERATED_BODY()
	FVector2D Start;
	FVector2D End;
};

UCLASS()
class INVENTORY_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(meta = (BindWidget))
	UBorder* GridBorder;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* GridCanvasPanel;


	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* WeightInfo;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY(EditDefaultsOnly)
	int32 TileSize;

	int32 DraggedTopLeftIndex;

private:

	TArray<FInventoryLine> Lines;

	UCanvasPanelSlot* GridBorderSlot;

	bool DrawDropLocation = false;

	USlateBrushAsset* SlateBrushAsset;

	FVector2D MousePositionOnGrid;

protected:

//	void SetInfoText() const;
	void NativeOnInitialized() override;
	void CreateLineSegments();
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
