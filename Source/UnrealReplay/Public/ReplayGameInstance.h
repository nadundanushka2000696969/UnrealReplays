// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkReplayStreaming.h"
#include "ReplayGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FS_ReplayInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString ReplayName;

    UPROPERTY(BlueprintReadOnly)
    FString FriendlyName;

    UPROPERTY(BlueprintReadOnly)
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadOnly)
    int32 LengthInMS;

    UPROPERTY(BlueprintReadOnly)
    bool bIsValid;

    FS_ReplayInfo(FString NewName, FString NewFriendlyName, FDateTime NewTimestamp, int32 NewLengthInMS)
    {
        ReplayName = NewName;
        FriendlyName = NewFriendlyName;
        Timestamp = NewTimestamp;
        LengthInMS = NewLengthInMS;
        bIsValid = true;
    }

    FS_ReplayInfo()
    {
        ReplayName = "Replay";
        FriendlyName = "Replay";
        Timestamp = FDateTime::MinValue();
        LengthInMS = 0;
        bIsValid = false;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplyFound, const TArray<FS_ReplayInfo>&, ReplayInfos);
UCLASS()
class UNREALREPLAY_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnReplyFound OnReplyFound;
    
    virtual void Init() override;

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

    UFUNCTION(BlueprintCallable, Category = "Replay")
    bool IsReplayActive() const;

    UFUNCTION(BlueprintCallable, Category = "Replay")
    void FindReplays();

private:
    TSharedPtr<INetworkReplayStreamer> EnumerateStreamsPtr;

    FEnumerateStreamsCallback OnEnumerateStreamsCompleteDelegate;

    void OnEnumerateStreamsComplete(const FEnumerateStreamsResult& StreamInfos);

    TArray<FS_ReplayInfo> m_AllReplays;

protected:

};
