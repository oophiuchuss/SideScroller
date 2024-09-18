// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SideScrollABPInterface.generated.h"

struct FGameplayTag;

// This class does not need to be modified.
UINTERFACE(Blueprintable, MinimalAPI)
class USideScrollABPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SIDESCROLL_API ISideScrollABPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Sets the current side direction.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Direction")
	bool SetSideDirectionTag(const FGameplayTag& NewSideDirection);

	// Sets the current Depth direction.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Direction")
	bool SetDepthDirectionTag(const FGameplayTag& NewDepthDirection);

};
