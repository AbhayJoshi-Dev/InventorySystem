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
	InstanceInteractableData.Action = Item->ItemTextData.InteractionText;
	InstanceInteractableData.Name = Item->ItemTextData.Name;
	InstanceInteractableData.Quantity = Item->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickup::TakePickup(const APlayerCharacter* Taker)
{
	if (IsPendingKillPending() && !Item) return;

	if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
	{
		bool Result = PlayerInventory->HandleAddItem(Item);

		if (Result)
		{
			Destroy();
		}
		/*switch (AddResult.OperationResult)
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

		UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());*/
	}
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(FDataTableRowHandle, RowName))
	{
		if (ItemRowHandle.IsNull()) return;

		if (const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString()))
		{
			PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);
		}
	}
}
#endif // WITH_EDITOR


void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemRowHandle.IsNull()) return;

	const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString());

	Item = NewObject<UItemBase>(this, BaseClass);

	Item->ID = ItemData->ID;
	Item->ItemType = ItemData->ItemType;
	Item->ItemQuality = ItemData->ItemQuality;
	Item->ItemNumericData = ItemData->ItemNumericData;
	Item->ItemTextData = ItemData->ItemTextData;
	Item->ItemAssetData = ItemData->ItemAssetData;

	Item->ItemNumericData.bIsStacakable = Item->ItemNumericData.MaxStackSize > 1;
	InQuantity <= 0 ? Item->SetQuantity(1) : Item->SetQuantity(InQuantity);

	PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);

	UpdateInteractableData();
}

void APickup::InitializeDrop(UItemBase* ItemToDrop)
{
	Item = ItemToDrop;
	Item->ItemNumericData.Weight = ItemToDrop->GetItemSingleWeight();
	Item->OwningInventory = nullptr;
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

