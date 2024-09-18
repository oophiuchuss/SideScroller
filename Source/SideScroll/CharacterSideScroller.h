// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterSideScroller.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SIDESCROLL_API ACharacterSideScroller : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterSideScroller();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateMovementTag(const FGameplayTag& NewTag, bool bIsSideMovement);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanTurn = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FGameplayTag SideDirectionTag;
	FGameplayTag DepthDirectionTag;
};
