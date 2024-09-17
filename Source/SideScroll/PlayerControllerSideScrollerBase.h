// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerSideScrollerBase.generated.h"

class UInputMappingContext;
class UInputAction;
class UEnhancedInputComponent;
class ACharacterSideScroller;
struct FInputActionValue;

/**
 *
 */
UCLASS(Abstract)
class SIDESCROLL_API APlayerControllerSideScrollerBase : public APlayerController
{
	GENERATED_BODY()

public:
	/** MappingContext */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	/** Move Input Action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction = nullptr;

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	// Used to handle move input
	void HandleMove(const FInputActionValue& InputActionValue);

private:
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacterSideScroller> PlayerCharacter = nullptr;
};
