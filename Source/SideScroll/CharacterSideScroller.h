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

	void Attack(const FGameplayTag& AttackTag);

	FVector GetCameraLocation();
	FVector GetCameraForwardVector();

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() { return bIsAttacking; }
	
	UFUNCTION(BlueprintCallable)
	bool IsTurning() { return bNeedsRotation; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanTurn = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotationSpeed = 5.0f;

	// Light attack montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|LightAttacks")
	UAnimMontage* LightAttackForwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|LightAttacks")
	UAnimMontage* LightAttackTopMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|LightAttacks")
	UAnimMontage* LightAttackBottomMontage;

	// Heavy attack montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|HeavyAttacks")
	UAnimMontage* HeavyAttackForwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|HeavyAttacks")
	UAnimMontage* HeavyAttackTopMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|HeavyAttacks")
	UAnimMontage* HeavyAttackBottomMontage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnAttackAnimationEnd(UAnimMontage* Montage, bool bInterrupted);
	bool CanAttack() const;
	TObjectPtr<UAnimMontage> SelectAttackByTag(const FGameplayTag& AttackTag) const;
	void RotateCharacterSmoothly(float TargetYaw);
	void UpdateCharacterRotation(float DeltaTime);


	float CharacterTargetYaw = 0.0f;
	bool bNeedsRotation = false;
	bool bIsAttacking = false;
	FGameplayTag FacingDirectionTag;
	FGameplayTag SideDirectionTag;
	FGameplayTag DepthDirectionTag;
	FGameplayTag ActionTag;
};
