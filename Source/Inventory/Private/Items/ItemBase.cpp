// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"

UItemBase::UItemBase()
{

}

UItemBase* UItemBase::CreateItemCopy() const
{
    UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

    ItemCopy->ID = this->ID;
    ItemCopy->Quantity = this->Quantity;
    ItemCopy->ItemQuality = this->ItemQuality;
    ItemCopy->ItemType = this->ItemType;
    ItemCopy->ItemTextData = this->ItemTextData;
    ItemCopy->ItemNumericData = this->ItemNumericData;
    ItemCopy->ItemStatistics = this->ItemStatistics;
    ItemCopy->ItemAssetData = this->ItemAssetData;

    return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
    if (Quantity != NewQuantity)
    {
        Quantity = FMath::Clamp(NewQuantity, 0, ItemNumericData.bIsStacakable ? ItemNumericData.MaxStackSize : 1);
    }
}

//void UItemBase::Use(APlayerCharacter* Character)
//{

//}
