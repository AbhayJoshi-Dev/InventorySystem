// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryToolTip.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Items/ItemBase.h"
#include "Components/TextBlock.h"

void UInventoryToolTip::NativeConstruct()
{
	Super::NativeConstruct();

	const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	switch (ItemBeingHovered->ItemType)
	{
	case EItemType::Armor:
		break;

	case EItemType::Weapon:
		break;

	case EItemType::Shield:
		break;

	case EItemType::Consumable:
		ItemType->SetText(FText::FromString("Consumable"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
	//	SellValue->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case EItemType::Quest:
		break;

	case EItemType::Mundane:
		ItemType->SetText(FText::FromString("Mundane Item"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
		UsageText->SetVisibility(ESlateVisibility::Collapsed);
	//	SellValue->SetVisibility(ESlateVisibility::Collapsed);
		break;

	default:
		break;
	}

	ItemName->SetText(ItemBeingHovered->ItemTextData.Name);
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
	ArmorRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ArmorRating));
	UsageText->SetText(ItemBeingHovered->ItemTextData.UsageText);
	ItemDescription->SetText(ItemBeingHovered->ItemTextData.Description);
	//SellValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.SellValue));
	StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));

	if (ItemBeingHovered->ItemNumericData.bIsStacakable)
	{
		MaxStackSizeText->SetText(FText::AsNumber(ItemBeingHovered->ItemNumericData.MaxStackSize));
	}
	else
	{
		MaxStackSizeText->SetVisibility(ESlateVisibility::Collapsed);
	}

}
