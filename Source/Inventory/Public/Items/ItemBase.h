// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemDataStructs.h"
#include "ItemBase.generated.h"

class UInventoryComponent;

UCLASS()
class INVENTORY_API UItemBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UInventoryComponent* OwningInventory;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FName ID;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(VisibleAnywhere, Category = "Item Data")
	FItemAssetData ItemAssetData;

	bool bIsCopy;
	bool bIsPickup;

	UItemBase();

	void ResetItemFlags();

	UFUNCTION(Category = "Item")
	UItemBase* CreateItemCopy() const;

	UFUNCTION(Category = "Item")
	FORCEINLINE	float GetItemStackWeight() const { return Quantity * ItemNumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE float GetItemSingleWeight() const { return ItemNumericData.Weight; }

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == ItemNumericData.MaxStackSize; }

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);
	//UPROPERTY(Category = "Item")
	//virtual void Use(APlayerCharacter* Character);
	
protected:

	bool operator == (const FName& OtherID) const 
	{
		return ID == OtherID;
	}

};
