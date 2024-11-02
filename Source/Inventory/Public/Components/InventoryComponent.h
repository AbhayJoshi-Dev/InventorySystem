// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No Item Added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial Amount Item Added"),
	IAR_AllItemAdded UMETA(DisplayName = "All Item Added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() :
		ActualAmountAdded(0),
		OperationResult(EItemAddResult::IAR_NoItemAdded),
		ResultMessage(FText::GetEmpty())
	{}

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	}

	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	}

	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	}

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	FOnInventoryUpdated OnInventoryUpdated;


	UInventoryComponent();

	UFUNCTION()
	FItemAddResult HandleAddItem(UItemBase* Item);

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

	FItemAddResult HandleNonStackableItems(UItemBase* Item);
	int32 HandleStackableItems(UItemBase* Item, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UItemBase* Item, int32 RequestedAddAmount);
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount);

	void AddNewItem(UItemBase* Item, int32 AmountToAdd);
};
