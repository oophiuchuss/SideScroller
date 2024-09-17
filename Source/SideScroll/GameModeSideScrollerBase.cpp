// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeSideScrollerBase.h"
#include "CharacterSideScroller.h"
#include "PlayerControllerSideScrollerBase.h"
#include "HUDSideScroller.h"

AGameModeSideScrollerBase::AGameModeSideScrollerBase()
{
	DefaultPawnClass = ACharacterSideScroller::StaticClass();
	PlayerControllerClass = APlayerControllerSideScrollerBase::StaticClass();
	HUDClass = AHUDSideScroller::StaticClass();
}
