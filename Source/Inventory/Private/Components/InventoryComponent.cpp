// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* Item)
{
	const int32 InitialRequestAddAmount = Item->Quantity;

	//handle non stackable items
	if (!Item->ItemNumericData.bIsStacakable)
	{
		return HandleNonStackableItems(Item);
	}

	//handle stackable items
	const int32 StackableAmountAdded = HandleStackableItems(Item, InitialRequestAddAmount);

	if (StackableAmountAdded == InitialRequestAddAmount)
	{
		return FItemAddResult::AddedAll(InitialRequestAddAmount, FText::Format(
			FText::FromString("Successfully added {0} {1} to the inventory."),
			InitialRequestAddAmount,
			Item->ItemTextData.Name));
	}

	if (StackableAmountAdded < InitialRequestAddAmount && StackableAmountAdded > 0)
	{
		return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
			FText::FromString("Partial amount of {0} to the inventory. Number added = {1}"),
			Item->ItemTextData.Name,
			InitialRequestAddAmount));
	}

	if (StackableAmountAdded <= 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
			Item->ItemTextData.Name));
	}

	//test
	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem failthrough error. GetOwner() check failed."));
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* Item) const
{
	if (Item)
	{
		if (InventoryContents.Contains(Item))
		{
			return Item;
		}
	}

	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* Item) const
{
	if (Item)
	{
		if (const TObjectPtr<UItemBase>* Result = InventoryContents.FindByKey(Item))
		{
			return *Result;
		}
	}

	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* Item) const
{
	if (const TObjectPtr<UItemBase>* Result = InventoryContents.FindByPredicate([&Item](const UItemBase* InventoryItem)
		{
			return InventoryItem->ID == Item->ID && InventoryItem->IsFullItemStack();
		}))
	{
		return *Result;
	}

	return nullptr;
	
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* Item)
{
	InventoryContents.RemoveSingle(Item);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* Item, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, Item->Quantity);

	Item->SetQuantity(Item->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ActualAmountToRemove * Item->GetItemSingleWeight();

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* Item, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(Item, AmountToSplit);
		AddNewItem(Item, AmountToSplit);
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* Item)
{
	//check if the input item has valid weight
	if (FMath::IsNearlyZero(Item->GetItemSingleWeight()) || Item->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."), Item->ItemTextData.Name));
	}

	//check if the item weight overflow inventory weight capacity
	if (InventoryTotalWeight + Item->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item would overflow weight limit."), Item->ItemTextData.Name));
	}

	//adding one more item would overflow inventory slot capacity
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."), Item->ItemTextData.Name));
	}

	AddNewItem(Item, 1);

	return FItemAddResult::AddedAll(1, FText::Format(
		FText::FromString("Successfully added a single {0} to the inventory."), Item->ItemTextData.Name));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* Item, int32 RequestedAddAmount)
{
	return int32();
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* Item, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / Item->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}

	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->ItemNumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::AddNewItem(UItemBase* Item, int32 AmountToAdd)
{
	UItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickup)
	{
		// item already a copy or is a world pickup
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// splitting or dragging to.from other inventory
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}
