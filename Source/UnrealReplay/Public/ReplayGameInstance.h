// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ReplayGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALREPLAY_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()
    // Start recording a replay
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void StartRecordingReplay(const FString& ReplayName);

    // Stop recording a replay
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void StopRecordingReplay();

    // Play a replay
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void PlayReplay(const FString& ReplayName);

    // Pause a replay
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void PauseReplay();

    // Resume a replay
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void ResumeReplay();

    // Rename a replay file
    UFUNCTION(BlueprintCallable, Category = "Replay")
    bool RenameReplay(const FString& OldReplayName, const FString& NewReplayName);

    // Get all saved replay files
    UFUNCTION(BlueprintCallable, Category = "Replay")
    TArray<FString> GetSavedReplays();

    // Jump to a specific time in a replay
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void JumpToReplayTime(float TimeInSeconds);

    // Get current replay time
    UFUNCTION(BlueprintCallable, Category = "Replay")
    float GetCurrentReplayTime() const;

    // Get total replay time
    UFUNCTION(BlueprintCallable, Category = "Replay")
    float GetTotalReplayTime() const;

private:

    bool IsReplayActive() const;
};
