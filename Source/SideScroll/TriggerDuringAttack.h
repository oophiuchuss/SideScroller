// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterSideScroller.h"
#include "EnhancedPlayerInput.h"

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "TriggerDuringAttack.generated.h"

/**
 * 
 */
UCLASS()
class SIDESCROLL_API UTriggerDuringAttack : public UInputTrigger
{
	GENERATED_BODY()

    virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override
    {
        // Retrieve the local player controller and pawn
        APlayerController* PlayerController = Cast<APlayerController>(PlayerInput->GetOuter());
        if (!PlayerController)
        {
            return ETriggerState::None; // No valid player controller
        }

        // Get the character (or any other pawn the controller possesses)
        ACharacterSideScroller* Character = Cast<ACharacterSideScroller>(PlayerController->GetPawn());
        if (!Character)
        {
            return ETriggerState::None; // No valid character
        }

        // Check if the character is currently attacking
        if (Character->IsAttacking())
        {
            return ETriggerState::Ongoing;  // Block input while attacking
        }

        return ETriggerState::Triggered;  // Allow input otherwise
    }
};
