// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"
#include "Components/InventoryComponent.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickup(false)
{

}

void UItemBase::ResetItemFlags()
{
    bIsCopy = false;
    bIsPickup = false;
}

UItemBase* UItemBase::CreateItemCopy() const
{
    UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

    ItemCopy->ID = this->ID;
    ItemCopy->ItemQuality = this->ItemQuality;
    ItemCopy->ItemType = this->ItemType;
    ItemCopy->ItemTextData = this->ItemTextData;
    ItemCopy->ItemNumericData = this->ItemNumericData;
    ItemCopy->ItemAssetData = this->ItemAssetData;

    ItemCopy->bIsCopy = true;

    return ItemCopy;
}

//void UItemBase::Use(APlayerCharacter* Character)
//{

//}
