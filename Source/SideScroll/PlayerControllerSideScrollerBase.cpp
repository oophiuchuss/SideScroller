// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerSideScrollerBase.h"
#include "CharacterSideScroller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerSideScrollerBase::HandleMove);
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

	if (PlayerCharacter)
	{
		
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
}
