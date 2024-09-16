// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "UserInterface/InventoryHUD.h"

APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(GetRootComponent());


	InteractionCheckDistance = 250.0f;
	InteractionCheckFrequency = 0.1f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	HUD = Cast<AInventoryHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart = GetPawnViewLocation();
	FVector TraceEnd = TraceStart + GetViewRotation().Vector() * InteractionCheckDistance;

	FCollisionQueryParams QueryParms;
	QueryParms.AddIgnoredActor(this);
	FHitResult HitResult;

	DrawDebugLine(GetWorld(), TraceStart - FVector(0.f, 0.f, 10.0f), TraceEnd, FColor::Red, false, 1.0f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParms))
	{
		if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			if(HitResult.GetActor() != InteractionData.CurrentInteractable)
			{
				FoundInteractable(HitResult.GetActor());
				return;	
			}

			if(HitResult.GetActor() == InteractionData.CurrentInteractable) return;
		}
	}

	NoInteractableFound();

}

void APlayerCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();

}

void APlayerCharacter::NoInteractableFound()
{
	if(IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if(InteractionData.CurrentInteractable)
	{
		if(IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}
	}

	HUD->HideInteractionWidget();

	InteractionData.CurrentInteractable = nullptr;
	TargetInteractable = nullptr;
}

void APlayerCharacter::BeginInteract()
{
	// verify nothing has changed with the interactable 
	PerformInteractionCheck();

	if (!InteractionData.CurrentInteractable) return;

	if (!IsValid(TargetInteractable.GetObject())) return;

	TargetInteractable->BeginInteract();

	if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
	{
		Interact();
	}
	else
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Interaction, this, &APlayerCharacter::Interact, TargetInteractable->InteractableData.InteractionDuration);
	}
}

void APlayerCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void APlayerCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if(IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
		PerformInteractionCheck();

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ACharacter::Jump);

		EnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Started, this, &APlayerCharacter::BeginInteract);

		EnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndInteract);
	}
}

