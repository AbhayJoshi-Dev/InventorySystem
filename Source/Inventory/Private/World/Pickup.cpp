// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"
#include "Items/ItemBase.h"
#include "Character/PlayerCharacter.h"
#include "Components/InventoryComponent.h"

APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);

}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
	
}

void APickup::Interact(APlayerCharacter* PlayerCharacter)
{
	if (!PlayerCharacter) return;

	TakePickup(PlayerCharacter);
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->ItemTextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->ItemTextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickup::TakePickup(const APlayerCharacter* Taker)
{
	if (IsPendingKillPending() && !ItemReference) return;

	if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
	{
		const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

		switch (AddResult.OperationResult)
		{
		case EItemAddResult::IAR_NoItemAdded:
			break;

		case EItemAddResult::IAR_PartialAmountItemAdded:
			UpdateInteractableData();
			Taker->UpdateInteractionWidget();
			break;

		case EItemAddResult::IAR_AllItemAdded:
			Destroy();
			break;

		default:
			break;
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
	}
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID))
	{
		if (!ItemDataTable) return;

		if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
		{
			PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);
		}
	}
}
#endif // WITH_EDITOR


void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (!ItemDataTable && DesiredItemID.IsNone()) return;

	const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

	ItemReference = NewObject<UItemBase>(this, BaseClass);

	ItemReference->ID = ItemData->ID;
	ItemReference->ItemType = ItemData->ItemType;
	ItemReference->ItemQuality = ItemData->ItemQuality;
	ItemReference->ItemNumericData = ItemData->ItemNumericData;
	ItemReference->ItemTextData = ItemData->ItemTextData;
	ItemReference->ItemAssetData = ItemData->ItemAssetData;

	ItemReference->ItemNumericData.bIsStacakable = ItemReference->ItemNumericData.MaxStackSize > 1;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

	PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);

	UpdateInteractableData();
}

void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->ItemNumericData.Weight = ItemToDrop->GetItemSingleWeight();
	ItemReference->OwningInventory = nullptr;
	PickupMesh->SetStaticMesh(ItemToDrop->ItemAssetData.Mesh);

	UpdateInteractableData();
}

void APickup::BeginFocus()
{
	if (!PickupMesh) return;

	PickupMesh->SetRenderCustomDepth(true);
}

void APickup::EndFocus()
{
	if (!PickupMesh) return;

	PickupMesh->SetRenderCustomDepth(false);
}

