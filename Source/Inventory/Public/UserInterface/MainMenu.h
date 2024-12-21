// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

UCLASS()
class INVENTORY_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:

	void NativeOnInitialized() override;
	void NativeConstruct() override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
