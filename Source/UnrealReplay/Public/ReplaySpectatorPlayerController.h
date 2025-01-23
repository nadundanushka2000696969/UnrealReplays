// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ReplaySpectatorPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class UNREALREPLAY_API AReplaySpectatorPlayerController : public APlayerController
{
	GENERATED_BODY()
	/** we must set some Pause-Behavior values in the ctor */
public:
	AReplaySpectatorPlayerController(const FObjectInitializer& ObjectInitializer);

};
