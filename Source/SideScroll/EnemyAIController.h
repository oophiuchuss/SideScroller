#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class SIDESCROLL_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    // AI Behavior
    void MaintainDistanceToPlayer();
    void AttackPlayer();
    void ReactToPlayerAttack();
    bool CanMoveToLocation(FVector Location);
    void StopAttacking();
    void ResetDodge();
    void UpdateMovementDirectionTags();
    void MoveTowardsPlayer();
    void ResetMovement();
    // Helper functions
    APawn* GetPlayerPawn() const;

    // Flags and Variables
    bool bIsAttacking;
    bool bIsDodging;
    FVector SafeLocation;
    UPROPERTY()
    APawn* TargetPlayer;

    // Parameters
    float AttackDistance = 150.0f; // 1.5 meters in Unreal units
    float ReactionTime = 1.0f; // Time to wait before attacking
};
