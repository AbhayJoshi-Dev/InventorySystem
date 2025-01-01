// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Pickup.generated.h"

class UItemBase;
class APlayerCharacter;

UCLASS()
class INVENTORY_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	

	APickup();

	void InitializePickup();

	void InitializeDrop(UItemBase* ItemToDrop);

	FORCEINLINE UItemBase* GetItemBase() { return Item; }

	void BeginFocus() override;
	void EndFocus() override;

protected:

	virtual void BeginPlay() override;

	void Interact(APlayerCharacter* PlayerCharacter) override;
	void UpdateInteractableData();
	void TakePickup(const APlayerCharacter* Taker);

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR


protected:

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	FDataTableRowHandle ItemRowHandle;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Reference")
	UItemBase* Item;

	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;

};
