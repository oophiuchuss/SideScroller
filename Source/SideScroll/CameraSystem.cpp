// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraSystem.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "CharacterSideScroller.h"

// Sets default values
ACameraSystem::ACameraSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	// Create the Camera Spline
	CameraSpline = CreateDefaultSubobject<USplineComponent>(TEXT("CameraSpline"));
	CameraSpline->SetupAttachment(RootComponent);

	// Create the LookAt Spline
	LookAtSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LookAtSpline"));
	LookAtSpline->SetupAttachment(RootComponent);

	// Create a camera and attach to the boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideScrollCamera"));
	Camera->SetupAttachment(RootComponent);
	// Set default follow speed
	CameraFollowSpeed = 5.0f;
}

// Called when the game starts or when spawned
void ACameraSystem::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		// Get the player's character
		Player = Cast<ACharacter>(PlayerController->GetPawn());

		if (Player)
		{
			PlayerController->SetViewTarget(this);
		}
	}
}

// Called every frame
void ACameraSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera(DeltaTime);
}

void ACameraSystem::UpdateCamera(float DeltaTime)
{
	if (!CameraSpline || !LookAtSpline || !Player) return;

	// Move the camera along the spline based on the player's position
	float CameraPlayerDistance = GetClosestDistanceOnSpline(CameraSpline);
	float LookAtPlayerDistance = GetClosestDistanceOnSpline(LookAtSpline);

	FVector CameraPosition = CameraSpline->GetLocationAtSplineInputKey(CameraPlayerDistance, ESplineCoordinateSpace::World);
	Camera->SetWorldLocation(FMath::VInterpTo(Camera->GetComponentLocation(), CameraPosition, DeltaTime, CameraFollowSpeed));

	// Get LookAt point along its spline
	FVector LookAtPosition = LookAtSpline->GetLocationAtSplineInputKey(LookAtPlayerDistance, ESplineCoordinateSpace::World);

	// Make the camera look at the LookAt point
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), LookAtPosition);
	Camera->SetWorldRotation(FMath::RInterpTo(Camera->GetComponentRotation(), LookAtRotation, DeltaTime, CameraFollowSpeed));
}

float ACameraSystem::GetClosestDistanceOnSpline(const TObjectPtr<USplineComponent> Spline) const
{
	if (!CameraSpline || !Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraSpline or Player is null."));
		return 0.0f;
	}

	FVector PlayerLocation = Player->GetActorLocation();

	// Find the closest input key on the spline to the player location
	float ClosestInputKey = Spline->FindInputKeyClosestToWorldLocation(PlayerLocation);

	// Optionally draw a debug point at the closest point on the spline
	/*FVector ClosestPoint = Spline->GetLocationAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::World);
	DrawDebugPoint(GetWorld(), ClosestPoint, 10.0f, FColor::Green, false, 0.0f);*/

	return ClosestInputKey;
}


