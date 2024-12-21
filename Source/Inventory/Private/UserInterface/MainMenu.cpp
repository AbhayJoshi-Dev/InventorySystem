// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/MainMenu.h"
#include "Components/InventoryComponent.h"
#include "Blueprint/DragDropOperation.h"
#include "Items/ItemBase.h"

void UMainMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
    Super::NativeConstruct();
}
bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{    
    if (InOperation->Payload)
    {
        if (UItemBase* Item = Cast<UItemBase>(InOperation->Payload))
        {
            Item->OwningInventory->DropItem(Item);
            return true;
        }
    }

    //if (ItemDragDrop->SourceItem)
    //{
    //    ItemDragDrop->SourceInventory->DropItem(ItemDragDrop->SourceItem);
    //    return true;
    //}

    return false;
}