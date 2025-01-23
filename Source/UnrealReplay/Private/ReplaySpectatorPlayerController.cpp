// Fill out your copyright notice in the Description page of Project Settings.

#include "ReplaySpectatorPlayerController.h"
#include "Engine/DemoNetDriver.h"
#include "GameFramework/SpectatorPawn.h"

AReplaySpectatorPlayerController::AReplaySpectatorPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;
}