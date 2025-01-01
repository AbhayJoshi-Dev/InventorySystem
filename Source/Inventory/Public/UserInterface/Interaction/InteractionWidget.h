// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class APlayerCharacter;
class UTextBlock;
class UProgressBar;
struct FInteractableData;

UCLASS()
class INVENTORY_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere)
	APlayerCharacter* PlayerCharacter;

public:

	void UpdateWidget(const FInteractableData* InteractableData) const;

protected:

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* ActionText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* KeyPressText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* InteractionProgressBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	float CurrentInteractionDuration;

protected:

	UFUNCTION()
	float UpdateInteractionProgress();

	void NativeOnInitialized() override;
	void NativeConstruct() override;
};
