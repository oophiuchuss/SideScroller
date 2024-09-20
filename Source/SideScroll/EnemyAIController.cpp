#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterSideScroller.h" // Assuming AICharacter is the class for your enemy
#include "TimerManager.h"
#include "GameplayTagsManager.h"

AEnemyAIController::AEnemyAIController()
{
    bIsAttacking = false;
    bIsDodging = false;
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    TargetPlayer = GetPlayerPawn();
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MaintainDistanceToPlayer();

    FVector Velocity = GetPawn()->GetVelocity();
    if (Velocity.SizeSquared() < KINDA_SMALL_NUMBER)  // Check if velocity is almost zero
    {
        ResetMovement();  // Reset movement tags if not moving
    }
}

void AEnemyAIController::MaintainDistanceToPlayer()
{

    if (!TargetPlayer) return;

    float DistanceToPlayer = FVector::Dist(TargetPlayer->GetActorLocation(), GetPawn()->GetActorLocation());

    if (DistanceToPlayer > AttackDistance)  // If more than 1.5 meters
    {
        MoveTowardsPlayer();
    }
    else
    {
        StopMovement();
        if (!bIsAttacking && !bIsDodging)
        {
            // Trigger attack after 1 second if within range
            GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEnemyAIController::AttackPlayer);
        }
    }
}

void AEnemyAIController::AttackPlayer()
{
    if (bIsAttacking || bIsDodging) return;

    ACharacterSideScroller* EnemyCharacter = Cast<ACharacterSideScroller>(GetPawn());
    if (!EnemyCharacter) return;

    bIsAttacking = true;

    // Play the attack animation
    EnemyCharacter->Attack(UGameplayTagsManager::Get().RequestGameplayTag(FName("Action.LightAttack.Right")));

    // Stop input during the attack for a set duration
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEnemyAIController::StopAttacking);
}

void AEnemyAIController::StopAttacking()
{
    bIsAttacking = false;
}

void AEnemyAIController::ReactToPlayerAttack()
{
    if (bIsAttacking) return; // If already attacking, ignore

    // Perform dodge or move away
    ACharacterSideScroller* EnemyCharacter = Cast<ACharacterSideScroller>(GetPawn());
    if (EnemyCharacter)
    {
        EnemyCharacter->Attack(UGameplayTagsManager::Get().RequestGameplayTag(FName("Action.LightAttack.Top")));
        bIsDodging = true;

        // After dodge, move away from the player
        FVector AwayFromPlayer = GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * -200.0f;
        if (CanMoveToLocation(AwayFromPlayer))
        {
            MoveToLocation(AwayFromPlayer);
        }

        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEnemyAIController::ResetDodge);
    }
}

void AEnemyAIController::ResetDodge()
{
    bIsDodging = false;
}

bool AEnemyAIController::CanMoveToLocation(FVector Location)
{
    FHitResult HitResult;
    FVector Start = GetPawn()->GetActorLocation();
    FVector End = Location;

    // Perform a line trace
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

    // If it hits something, return false, indicating the enemy can't move there
    return !bHit;
}

APawn* AEnemyAIController::GetPlayerPawn() const
{
    return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEnemyAIController::UpdateMovementDirectionTags()
{
    ACharacterSideScroller* EnemyCharacter = Cast<ACharacterSideScroller>(GetPawn());
    if (!EnemyCharacter || !TargetPlayer)
    {
        return;
    }

    FVector AICharacterLocation = EnemyCharacter->GetActorLocation();
    FVector PlayerLocation = TargetPlayer->GetActorLocation();

    // Calculate movement vector from AI to Player
    FVector MovementVector = PlayerLocation - AICharacterLocation;
    MovementVector.Normalize();  // Normalize the vector to just get direction

    // Get the forward vector of the camera
    FVector CameraForward = EnemyCharacter->GetCameraForwardVector();

    // Determine the right vector based on camera's rotation
    FVector CameraRight = FVector::CrossProduct(FVector::UpVector, CameraForward).GetSafeNormal();

    // Check the AI's position relative to the player and camera direction
    float DotForward = FVector::DotProduct(MovementVector, CameraForward);
    float DotRight = FVector::DotProduct(MovementVector, CameraRight);

    // Update movement tags based on dot products
    if (DotRight > 0)  // AI is moving to the right of the camera
    {
        EnemyCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Right")), true);
    }
    else if (DotRight < 0)  // AI is moving to the left of the camera
    {
        EnemyCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Left")), true);
    }
    else if (DotForward > 0)  // AI is moving towards the camera
    {
        EnemyCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Into")), false);
    }
    else if (DotForward < 0)  // AI is moving away from the camera
    {
        EnemyCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.Towards")), false);
    }
}

void AEnemyAIController::MoveTowardsPlayer()
{
    if (!TargetPlayer)
        return;

    // Update movement direction tags based on player position
    UpdateMovementDirectionTags();

    // Move the AI towards the player
    MoveToActor(TargetPlayer, AttackDistance);  // Move towards player within 1.5 meters
}

void AEnemyAIController::ResetMovement()
{
    ACharacterSideScroller* EnemyCharacter = Cast<ACharacterSideScroller>(GetPawn());
    if (!EnemyCharacter)
    {
        return;
    }

    // Reset all movement direction tags to None
    EnemyCharacter->UpdateMovementTag(UGameplayTagsManager::Get().RequestGameplayTag(FName("Direction.None")), false);
}