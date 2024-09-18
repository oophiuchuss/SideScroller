// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerSideScrollerBase.h"
#include "CharacterSideScroller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameplayTagsManager.h"
#include "GameFramework/CharacterMovementComponent.h"

void APlayerControllerSideScrollerBase::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// Store a reference of player character
	PlayerCharacter = Cast<ACharacterSideScroller>(aPawn);
	checkf(PlayerCharacter, TEXT("APlayerControllerSideScrollerBase should posses only ACharacterSideScroller derived classes"));

	// Get a reference of enhanced input component
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(PlayerCharacter, TEXT("Unable to get EnhancedInputComponent reference"));

	// Get the local player subsystem
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSusbsytem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSusbsytem, TEXT("Unable to get EnhancedInputLocalPlayerSubsystem reference"));

	// Clear previous mappings
	checkf(DefaultMappingContext, TEXT("DefaultMappingContext was not specified"));
	InputSusbsytem->ClearAllMappings();
	InputSusbsytem->AddMappingContext(DefaultMappingContext, 0);


	// Bind the input actions
	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerSideScrollerBase::HandleMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerControllerSideScrollerBase::ResetMovement);
	}

	if (LightAttackAction)
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &APlayerControllerSideScrollerBase::HandleLightAttack);

	if (HeavyAttackAction)
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &APlayerControllerSideScrollerBase::HandleHeavyAttack);

}

void APlayerControllerSideScrollerBase::OnUnPossess()
{
	// Clear all bindings before unpossessing the player
	EnhancedInputComponent->ClearActionBindings();

	Super::OnUnPossess();
}

void APlayerControllerSideScrollerBase::HandleMove(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if (!PlayerCharacter)
		return;

	if (MovementVector.X > 0)
	{
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = PlayerCharacter->bCanTurn;
		PlayerCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Right")), true);
	}
	else if (MovementVector.X < 0)
	{
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = PlayerCharacter->bCanTurn;
		PlayerCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Left")), true);
	}
	else if (MovementVector.Y > 0)
	{

		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Into")), false);
	}
	else if (MovementVector.Y < 0)
	{
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Towards")), false);
	}

	// find out which way is forward
	const FRotator Rotation = PlayerCameraManager->GetCameraRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	PlayerCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
	PlayerCharacter->AddMovementInput(RightDirection, MovementVector.X);


}

void APlayerControllerSideScrollerBase::ResetMovement()
{
	PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.None")), false);
}

void APlayerControllerSideScrollerBase::HandleLightAttack(const FInputActionValue& InputActionValue)
{
	FVector2D AttackVector = InputActionValue.Get<FVector2D>();

	FString Direction = "";

	if (AttackVector.X > 0)
	{
		Direction = "Up";
	}
	else if (AttackVector.X < 0)
	{
		Direction = "Down";
	}
	else if (AttackVector.Y > 0)
	{
		Direction = "Right";
	}
	else if (AttackVector.Y < 0)
	{
		Direction = "Left";
	}

	UE_LOG(LogTemp, Warning, TEXT("Light: %s"), *Direction);
}

void APlayerControllerSideScrollerBase::HandleHeavyAttack(const FInputActionValue& InputActionValue)
{
	FVector2D AttackVector = InputActionValue.Get<FVector2D>();

	FString Direction = "";

	if (AttackVector.X > 0)
	{
		Direction = "Up";
	}
	else if (AttackVector.X < 0)
	{
		Direction = "Down";
	}
	else if (AttackVector.Y > 0)
	{
		Direction = "Right";
	}
	else if (AttackVector.Y < 0)
	{
		Direction = "Left";
	}

	UE_LOG(LogTemp, Warning, TEXT("Heavy: %s"), *Direction);
}
