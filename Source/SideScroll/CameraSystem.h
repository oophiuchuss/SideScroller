// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraSystem.generated.h"

class USplineComponent;
class UCameraComponent;
class ACharacterSideScroller;
class USceneComponent;

UCLASS()
class SIDESCROLL_API ACameraSystem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraSystem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Spline for camera movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USplineComponent> CameraSpline;

	// Spline for the camera LookAt point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USplineComponent> LookAtSpline;

	// Side view camera component
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Function to update camera position and rotation along the spline
	void UpdateCamera(float DeltaTime);

	// Function to find the closest point on the spline to the player
	float GetClosestDistanceOnSpline(const TObjectPtr<USplineComponent> Spline) const;

	// Player reference
	UPROPERTY()
	TObjectPtr<ACharacter> Player;

	UPROPERTY()
	TObjectPtr<USceneComponent> RootSceneComponent;

	// Speed at which the camera follows the player
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float CameraFollowSpeed;
};
