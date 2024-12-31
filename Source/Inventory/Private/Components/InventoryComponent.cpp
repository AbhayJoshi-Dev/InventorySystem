// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "World/Pickup.h"

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryContents.Reserve(Rows * Columns);

	InventoryContents.Init(nullptr, Rows * Columns);
}

bool UInventoryComponent::HandleAddItem(UItemBase* Item)
{
	if (!Item) return false;

	// for loop for all array elements
	for(int32 i = 0; i < InventoryContents.Max(); i++)
	{
		// is room available
		if (IsRoomAvailable(Item, i))
		{
			AddNewItem(Item, i);

			return true;
		}
	}
	return false;
}

//FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* Item)
//{
//	const int32 InitialRequestAddAmount = Item->Quantity;
//
//	//handle non stackable items
//	if (!Item->ItemNumericData.bIsStacakable)
//	{
//		return HandleNonStackableItems(Item);
//	}
//
//	//handle stackable items
//	const int32 StackableAmountAdded = HandleStackableItems(Item, InitialRequestAddAmount);
//
//	if (StackableAmountAdded == InitialRequestAddAmount)
//	{
//		return FItemAddResult::AddedAll(InitialRequestAddAmount, FText::Format(
//			FText::FromString("Successfully added {0} {1} to the inventory."),
//			InitialRequestAddAmount,
//			Item->ItemTextData.Name));
//	}
//
//	if (StackableAmountAdded < InitialRequestAddAmount && StackableAmountAdded > 0)
//	{
//		return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
//			FText::FromString("Partial amount of {0} to the inventory. Number added = {1}"),
//			Item->ItemTextData.Name,
//			InitialRequestAddAmount));
//	}
//
//	if (StackableAmountAdded <= 0)
//	{
//		return FItemAddResult::AddedNone(FText::Format(
//			FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
//			Item->ItemTextData.Name));
//	}
//
//	//test
//	check(false);
//	return FItemAddResult::AddedNone(FText::FromString("TryAddItem failthrough error. GetOwner() check failed."));
//}

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
			return InventoryItem->ID == Item->ID && !InventoryItem->IsFullItemStack();
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

void UInventoryComponent::RemoveItem(UItemBase* ItemToRemove)
{
	if (!InventoryContents.Contains(ItemToRemove)) return;

	InventoryTotalWeight -= ItemToRemove->GetItemSingleWeight();

	// TODO: could be done better
	while (InventoryContents.Contains(ItemToRemove))
	{
		int32 index = InventoryContents.IndexOfByPredicate(
			[&](const UItemBase* Item)
			{
				return Item == ItemToRemove;
			}
		);

		InventoryContents[index] = nullptr;
	}

	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::DropItem(UItemBase* ItemToDrop)
{

	FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
	SpawnParams.bNoFail = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//TODO: update drop spawn location based on inventory's owner, current setup is only for player
	const FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f;
	const FTransform SpawnTransform(GetOwner()->GetActorRotation(), SpawnLocation);

	//removes content from inventory and broadcast to refresh inventory
	RemoveItem(ItemToDrop);

	APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);
	Pickup->InitializeDrop(ItemToDrop);
}

void UInventoryComponent::SplitExistingStack(UItemBase* Item, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(Item, AmountToSplit);
		AddNewItem(Item, AmountToSplit);
	}
}

TMap<UItemBase*, FTile> UInventoryComponent::GetUniqueItems()
{
	TMap<UItemBase*, FTile> Items;

	for (int i = 0; i < InventoryContents.Num(); i++)
	{
		// element is null or invalid
		if (!InventoryContents[i]) continue;

		// element already in the map
		if (Items.Contains(InventoryContents[i])) continue;

		Items.Add(InventoryContents[i], IndexToTile(i));
	}

	return Items;
}

//FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* Item)
//{
//	//check if the input item has valid weight
//	if (FMath::IsNearlyZero(Item->GetItemSingleWeight()) || Item->GetItemSingleWeight() < 0)
//	{
//		return FItemAddResult::AddedNone(FText::Format(
//			FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."), Item->ItemTextData.Name));
//	}
//
//	//check if the item weight overflow inventory weight capacity
//	if (InventoryTotalWeight + Item->GetItemSingleWeight() > GetWeightCapacity())
//	{
//		return FItemAddResult::AddedNone(FText::Format(
//			FText::FromString("Could not add {0} to the inventory. Item would overflow weight limit."), Item->ItemTextData.Name));
//	}
//
//	//adding one more item would overflow inventory slot capacity
//	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
//	{
//		return FItemAddResult::AddedNone(FText::Format(
//			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."), Item->ItemTextData.Name));
//	}
//
//	AddNewItem(Item, 1);
//
//	return FItemAddResult::AddedAll(1, FText::Format(
//		FText::FromString("Successfully added a single {0} to the inventory."), Item->ItemTextData.Name));
//}

int32 UInventoryComponent::HandleStackableItems(UItemBase* Item, int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(Item->GetItemStackWeight()))
	{
		// invalid item weight, nothing added
		return 0;
	}

	int32 AmountToDistribute = RequestedAddAmount;

	//check if the item already exists in the inventory  and is not a full stack
	UItemBase* ExistingItemStack = FindNextPartialStack(Item);

	//distribute item stack over existing stacks
	while (ExistingItemStack)
	{
		// calculate how many items needed to make full stack
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		//calculate how many items can fit in inventory based on weight
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		if (WeightLimitAddAmount > 0)
		{
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);
			InventoryTotalWeight += (ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);

			AmountToDistribute -= WeightLimitAddAmount;

			Item->SetQuantity(AmountToDistribute);

			//  inventory max weight will reached if another item added
			if (InventoryTotalWeight + ExistingItemStack->GetItemSingleWeight() > InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				//weight limit, can't add more

				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			// partial stack found but nothing ca be added
			return 0;
		}

		if (AmountToDistribute <= 0)
		{
			// everithing was distributed across existing stacks
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		ExistingItemStack = FindNextPartialStack(Item);
	}

	// no partial stacks found, check if new stack can be added
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		// calculate how many items can fit in inventory based on weight
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(Item, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			// check if we can add all item or weight limit reached
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				AmountToDistribute -= WeightLimitAddAmount;
				Item->SetQuantity(AmountToDistribute);

				//create copy because only a partial stack being added
				//AddNewItem(Item->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			// full remainder of stack added
			//AddNewItem(Item, AmountToDistribute);
			return RequestedAddAmount;
		}

		//no partial stacks and no extra weight capacity
		return RequestedAddAmount - AmountToDistribute;
	}

	//no existing stack left or no extra slots capacity
	return 0;
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* Item, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / Item->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calculating Weight Add Amount"));
		return RequestedAddAmount;
	}

	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->ItemNumericData.MaxStackSize - StackableItem->Quantity;

	UE_LOG(LogTemp, Warning, TEXT("Calculating Amount for full Stack"));

	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::AddNewItem(UItemBase* Item, int32 TopLeftIndex)
{
	const FTile Tile = IndexToTile(TopLeftIndex);

	//horizontal grid
	for (int32 j = Tile.X; j < Tile.X + Item->GetDimensions().X; j++)
	{
		//vertical grid
		for (int32 k = Tile.Y; k < Tile.Y + Item->GetDimensions().Y; k++)
		{
			InventoryContents[TileToIndex(FTile(j, k))] = Item;
		}
	}

	OnInventoryUpdated.Broadcast();
	Item->OwningInventory = this;
}

//void UInventoryComponent::AddNewItem(UItemBase* Item, int32 AmountToAdd)
//{
//	UItemBase* NewItem;
//
//	if (Item->bIsCopy || Item->bIsPickup)
//	{
//		// item already a copy or is a world pickup
//		NewItem = Item;
//		NewItem->ResetItemFlags();
//	}
//	else
//	{
//		// splitting or dragging to.from other inventory
//		NewItem = Item->CreateItemCopy();
//	}
//
//	NewItem->OwningInventory = this;
//	NewItem->SetQuantity(AmountToAdd);
//
//	InventoryContents.Add(NewItem);
//	InventoryTotalWeight += NewItem->GetItemStackWeight();
//	OnInventoryUpdated.Broadcast();
//}

const FTile UInventoryComponent::IndexToTile(int32 Index) const
{
	return FTile( Index % Columns, Index / Columns);
}

const int32 UInventoryComponent::TileToIndex(FTile Tile) const
{
	return Tile.X + Tile.Y * Columns;
}

const bool UInventoryComponent::IsTileValid(FTile Tile) const
{
	return (Tile.X >= 0 && Tile.Y >=0) && (Tile.X < Columns && Tile.Y < Rows);
}

UE_DISABLE_OPTIMIZATION

bool UInventoryComponent::IsRoomAvailable(UItemBase* Item, int32 TopLeftIndex)
{
	const FTile Tile = IndexToTile(TopLeftIndex);

	//horizontal grid
	for (int32 j = Tile.X; j < Tile.X + Item->GetDimensions().X; j++)
	{
		//vertical grid
		for (int32 k = Tile.Y; k < Tile.Y + Item->GetDimensions().Y; k++)
		{
			if (!IsTileValid(FTile(j, k))) return false;

			int32 index = TileToIndex(FTile(j, k));

			// no room available on the tile
			if (InventoryContents[index]) return false;


		}
	}

	return true;
}
UE_ENABLE_OPTIMIZATION