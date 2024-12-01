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
struct FInventoryLine;

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

	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* CapacityInfo;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY(EditDefaultsOnly)
	float TileSize;

private:

	TArray<FInventoryLine> Lines;

	UCanvasPanelSlot* GridBorderSlot;

protected:

//	void SetInfoText() const;
	void NativeOnInitialized() override;
	void CreateLineSegments();
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
