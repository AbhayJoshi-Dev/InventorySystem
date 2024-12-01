// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;


USTRUCT(BlueprintType)
struct FInventoryLine
{
	GENERATED_BODY()
	FVector2D Start;
	FVector2D End;
};

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()

	int32 X;
	int32 Y;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnInventoryUpdated OnInventoryUpdated;


	UInventoryComponent();

	UFUNCTION()
	bool HandleAddItem(UItemBase* Item);

	UFUNCTION()
	UItemBase* FindMatchingItem(UItemBase* Item) const;
	UFUNCTION()
	UItemBase* FindNextItemByID(UItemBase* Item) const;
	UFUNCTION()
	UItemBase* FindNextPartialStack(UItemBase* Item) const;
	UFUNCTION()
	void RemoveSingleInstanceOfItem(UItemBase* Item);
	UFUNCTION()
	int32 RemoveAmountOfItem(UItemBase* Item, int32 DesiredAmountToRemove);
	UFUNCTION()
	void SplitExistingStack(UItemBase* Item, const int32 AmountToSplit);

	TMap<UItemBase*, FTile> GetUniqueItems();

	UFUNCTION()
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; }
	UFUNCTION()
	FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; }
	UFUNCTION()
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; }
	UFUNCTION()
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; }

	UFUNCTION()
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; }
	UFUNCTION()
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; }

	UFUNCTION()
	FORCEINLINE int32 GetRows() const { return Rows; }

	UFUNCTION()
	FORCEINLINE int32 GetColumns() const { return Columns; }

protected:

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float InventoryTotalWeight;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 InventorySlotsCapacity;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float InventoryWeightCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Rows;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Columns;

protected:

	virtual void BeginPlay() override;

//	FItemAddResult HandleNonStackableItems(UItemBase* Item);
	int32 HandleStackableItems(UItemBase* Item, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UItemBase* Item, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount);

	void AddNewItem(UItemBase* Item, int32 TopLeftIndex);

	const FTile IndexToTile(int32 Index) const;

	const int32 TileToIndex(FTile Tile) const;

	const bool IsTileValid(FTile Tile) const;

	bool IsRoomAvailable(UItemBase* Item, int32 TopLeftIndex);
};
