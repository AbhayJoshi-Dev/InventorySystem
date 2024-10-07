// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Inventory/InventoryToolTip.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "Items/ItemBase.h"
#include "Components/TextBlock.h"

void UInventoryToolTip::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference())
	{
		switch (ItemBeingHovered->ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemType->SetColorAndOpacity(FLinearColor::Gray);
			break;

		case EItemQuality::Common:
			ItemType->SetColorAndOpacity(FLinearColor::White);
			break;

		case EItemQuality::Quality:
			ItemType->SetColorAndOpacity(FLinearColor::Green);
			break;

		case EItemQuality::Masterwork:
			ItemType->SetColorAndOpacity(FLinearColor::Blue);
			break;

		case EItemQuality::Grandmaster:
			ItemType->SetColorAndOpacity(FLinearColor::Red);
			break;

		default:
			break;
		}


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
			break;

		case EItemType::Quest:
			break;

		case EItemType::Mundane:
			ItemType->SetText(FText::FromString("Mundane"));
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
			ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
			UsageText->SetVisibility(ESlateVisibility::Collapsed);
			break;

		default:
			break;
		}

		ItemName->SetText(ItemBeingHovered->ItemTextData.Name);
		DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
		ArmorRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ArmorRating));
		UsageText->SetText(ItemBeingHovered->ItemTextData.UsageText);
		ItemDescription->SetText(ItemBeingHovered->ItemTextData.Description);

		const FString WeightInfo = { "Weight: " + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight()) };
		StackWeight->SetText(FText::FromString(WeightInfo));

		if (ItemBeingHovered->ItemNumericData.bIsStacakable)
		{
			const FString StackInfo = { "Max stack size: " + FString::FromInt(ItemBeingHovered->ItemNumericData.MaxStackSize) };
			MaxStackSizeText->SetText(FText::FromString(StackInfo));
		}
		else
		{
			MaxStackSizeText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
