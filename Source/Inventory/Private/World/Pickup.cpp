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

	InitializePickup();
	
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


void APickup::InitializePickup()
{
	if (ItemRowHandle.IsNull()) return;

	const FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString());

	Item = NewObject<UItemBase>(this, UItemBase::StaticClass());

	Item->ID = ItemData->ID;
	Item->ItemType = ItemData->ItemType;
	Item->ItemQuality = ItemData->ItemQuality;
	Item->ItemNumericData = ItemData->ItemNumericData;
	Item->ItemTextData = ItemData->ItemTextData;
	Item->ItemAssetData = ItemData->ItemAssetData;

	PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);

	UpdateInteractableData();
}

void APickup::InitializeDrop(UItemBase* ItemToDrop)
{
	Item = ItemToDrop;
	Item->ItemNumericData.Weight = ItemToDrop->GetWeight();
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

