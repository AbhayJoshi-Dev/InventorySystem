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

void UInventoryComponent::RemoveItem(UItemBase* ItemToRemove)
{
	if (!InventoryContents.Contains(ItemToRemove)) return;

	InventoryTotalWeight -= ItemToRemove->GetWeight();

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

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* Item, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / Item->GetWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calculating Weight Add Amount"));
		return RequestedAddAmount;
	}

	return WeightMaxAddAmount;
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

FTile UInventoryComponent::GetTopLeftTileOfItem(UItemBase* Item)
{
	// TODO: this method of finding top left tile might give errors, its finds first tile and return
	for (int32 i = 0; i < InventoryContents.Max(); i++)
	{
		if (InventoryContents[i] == Item)
		{
			return IndexToTile(i);
		}
	}

	return FTile();
}

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