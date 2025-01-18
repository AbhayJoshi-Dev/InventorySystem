// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragItemVisual.generated.h"

class UBorder;
class UImage;
class UTextBlock;
class USizeBox;

UCLASS()
class INVENTORY_API UDragItemVisual : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category = "Drag Item Visual", meta = (BindWidget))
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, Category = "Drag Item Visual", meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(VisibleAnywhere, Category = "Drag Item Visual", meta = (BindWidget))
	USizeBox* BackgroundSizeBox;

public:

	FORCEINLINE FVector2D GetMousePosWhenDragged() const { return MousePosWhenDragged; }

	void SetImageAndSize(UMaterialInterface* Image, FVector2D Size);

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	UFUNCTION()
	FSlateBrush UpdateBrush();
private:

	FVector2D MousePosWhenDragged;

	UMaterialInterface* ImageMaterial;
	FVector2D ImageSize;
};
