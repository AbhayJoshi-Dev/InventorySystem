// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryPanel.h"
#include "Character/PlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());

	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (!InventoryReference) return;

		InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
		SetInfoText();
	}
}

void UInventoryPanel::RefreshInventory()
{
	if (!InventoryReference || !InventorySlotClass) return;

	InventoryWrapBox->ClearChildren();

	for (UItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
	{
		UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
		ItemSlot->SetItemReference(InventoryItem);
			
		InventoryWrapBox->AddChildToWrapBox(ItemSlot);
	}

	SetInfoText();
}

void UInventoryPanel::SetInfoText() const
{
	const FString WeightInfoValue = { FString::SanitizeFloat(InventoryReference->GetInventoryTotalWeight()) + "/"
										+ FString::SanitizeFloat(InventoryReference->GetWeightCapacity()) };

	WeightInfo->SetText(FText::FromString(WeightInfoValue));

	const FString CapacityInfoValue = { FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
										+ FString::FromInt(InventoryReference->GetSlotsCapacity()) };

	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on Inventory Panel."));

		return true;
	}

	return false;
}
