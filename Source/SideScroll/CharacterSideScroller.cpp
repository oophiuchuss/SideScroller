// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSideScroller.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "SideScrollABPInterface.h"
#include "GameplayTagsManager.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
ACharacterSideScroller::ACharacterSideScroller()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

// Called when the game starts or when spawned
void ACharacterSideScroller::BeginPlay()
{
	Super::BeginPlay();

	UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Right")), true);
	UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.None")), false);
}


// Called every frame
void ACharacterSideScroller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterSideScroller::UpdateMovementTag(const FGameplayTag& NewTag, bool bIsSideMovement)
{
	if (bIsSideMovement)
	{
		if (SideDirectionTag == NewTag)
			return;

		SideDirectionTag = NewTag;

		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

		checkf(AnimInstance->GetClass()->ImplementsInterface(USideScrollABPInterface::StaticClass()),
			TEXT("SetParkourAction: AnimInstance does not implement the ABP interface"));

		ISideScrollABPInterface* ABPInterface = Cast<ISideScrollABPInterface>(AnimInstance);
		ABPInterface->Execute_SetSideDirectionTag(AnimInstance, SideDirectionTag);
	}
	else
	{
		if (DepthDirectionTag == NewTag)
			return;

		DepthDirectionTag = NewTag;
		

		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

		checkf(AnimInstance->GetClass()->ImplementsInterface(USideScrollABPInterface::StaticClass()),
			TEXT("SetParkourAction: AnimInstance does not implement the ABP interface"));

		ISideScrollABPInterface* ABPInterface = Cast<ISideScrollABPInterface>(AnimInstance);
		ABPInterface->Execute_SetDepthDirectionTag(AnimInstance, DepthDirectionTag);
	}

	UE_LOG(LogTemp, Log, TEXT("Updated Tag: %s"), *NewTag.ToString());
}

