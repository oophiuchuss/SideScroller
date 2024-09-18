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
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	bIsAttacking = false;
}

void ACharacterSideScroller::Attack(const FGameplayTag& AttackTag)
{
	if (!CanAttack() || AttackTag.ToString().Contains("None"))
		return;

	if (SideDirectionTag.ToString().Contains("Left") && AttackTag.ToString().Contains("Right"))
	{
		UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Right")), true);
	}
	else if (SideDirectionTag.ToString().Contains("Right") && AttackTag.ToString().Contains("Left"))
	{
		UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Left")), true);
	}

	ActionTag = AttackTag;

	bIsAttacking = true;

	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	UAnimMontage* SelectedMontage = SelectAttackByTag(ActionTag);

	checkf(SelectedMontage, TEXT("Montage to play is invalid. Check tag names and montages in character. Tag name: %s"), *AttackTag.ToString());

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	checkf(AnimInstance, TEXT("Couldn't find animation instance from character mesh."));

	if (AnimInstance->Montage_IsPlaying(nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Another montage is already playing, new montage won't be played."));
		return;
	}
	
	float PlayRate = AnimInstance->Montage_Play(SelectedMontage);
	if (PlayRate == 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Montage %s failed to play."), *SelectedMontage->GetName());
		return;
	}
}

// Called when the game starts or when spawned
void ACharacterSideScroller::BeginPlay()
{
	Super::BeginPlay();

	UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Right")), true);
	UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.None")), false);
	ActionTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Action.None"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	checkf(AnimInstance, TEXT("Couldn't find animation instance from character mesh."));

	AnimInstance->OnMontageBlendingOut.AddDynamic(this, &ACharacterSideScroller::OnAttackAnimationEnd);
}

void ACharacterSideScroller::OnAttackAnimationEnd(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	ActionTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Action.None"));
}

bool ACharacterSideScroller::CanAttack() const
{
	return ActionTag.ToString().Contains("None");
}

TObjectPtr<UAnimMontage> ACharacterSideScroller::SelectAttackByTag(const FGameplayTag& AttackTag) const
{
	if (AttackTag.ToString().Contains("LightAttack"))
	{
		if (AttackTag.ToString().Contains("Top"))
			return LightAttackTopMontage;

		if (AttackTag.ToString().Contains("Bottom"))
			return LightAttackBottomMontage;

		if (AttackTag.ToString().Contains("Left") || AttackTag.ToString().Contains("Right"))
			return LightAttackForwardMontage;

	}
	else if (AttackTag.ToString().Contains("HeavyAttack"))
	{
		if (AttackTag.ToString().Contains("Top"))
			return HeavyAttackTopMontage;

		if (AttackTag.ToString().Contains("Bottom"))
			return HeavyAttackBottomMontage;

		if (AttackTag.ToString().Contains("Left") || AttackTag.ToString().Contains("Right"))
			return HeavyAttackForwardMontage;
	}

	return nullptr;
}

void ACharacterSideScroller::RotateCharacterSmoothly(float TargetYaw)
{
	if (!bNeedsRotation)
		return;

	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation = FRotator(CurrentRotation.Pitch, TargetYaw, CurrentRotation.Roll);

	// Smoothly interpolate rotation
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

	SetActorRotation(NewRotation);

	// Check if we have reached the target yaw
	if (FMath::Abs(NewRotation.Yaw - TargetYaw) < 1.0f)
	{
		bNeedsRotation = false;
	}
}


// Called every frame
void ACharacterSideScroller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateCharacterSmoothly(CharacterTargetYaw);
	UpdateCharacterRotation(DeltaTime);
}

void ACharacterSideScroller::UpdateMovementTag(const FGameplayTag& NewTag, bool bIsSideMovement)
{
	if (bIsSideMovement)
	{
		if (SideDirectionTag == NewTag)
			return;

		if (bCanTurn && !NewTag.ToString().Contains("None") && !SideDirectionTag.ToString().Contains("None"))
		{
			CharacterTargetYaw = GetActorRotation().Yaw + 180.0f;
			bNeedsRotation = true;
		}

		SideDirectionTag = NewTag;

		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

		checkf(AnimInstance->GetClass()->ImplementsInterface(USideScrollABPInterface::StaticClass()),
			TEXT("UpdateMovementTag: AnimInstance does not implement the ABP interface"));

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
			TEXT("UpdateMovementTag: AnimInstance does not implement the ABP interface"));

		ISideScrollABPInterface* ABPInterface = Cast<ISideScrollABPInterface>(AnimInstance);
		ABPInterface->Execute_SetDepthDirectionTag(AnimInstance, DepthDirectionTag);
	}

	UE_LOG(LogTemp, Log, TEXT("Updated Direction Tag: %s"), *NewTag.ToString());
}

void ACharacterSideScroller::UpdateCharacterRotation(float DeltaTime)
{
	AActor* ViewTarget = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewTarget();
	if (!ViewTarget)
		return;

	UCameraComponent* CameraComponent = ViewTarget->FindComponentByClass<UCameraComponent>();
	
	if (IsTurning() || !CameraComponent) return;

	// Get the right vector of the camera (which is perpendicular to the camera's forward direction)
	FVector CameraRightVector = CameraComponent->GetRightVector() * (SideDirectionTag.ToString().Contains("Right") ? 1.0f : -1.0f);

	// Calculate the target rotation for the character, using the camera's right vector as the forward direction
	FRotator TargetRotation = CameraRightVector.ToOrientationRotator();

	// Interpolate between the character's current rotation and the target rotation
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 3.0f); // Adjust the interpolation speed as needed

	// Apply the new rotation to the character
	SetActorRotation(NewRotation);
}