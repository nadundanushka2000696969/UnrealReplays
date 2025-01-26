// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayGameInstance.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/World.h"

void UReplayGameInstance::Init()
{
    Super::Init();

    // create a ReplayStreamer for FindReplays() and DeleteReplay(..)
    EnumerateStreamsPtr = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();
    // Link FindReplays() delegate to function
    OnEnumerateStreamsCompleteDelegate = FEnumerateStreamsCallback::CreateUObject(this, &UReplayGameInstance::OnEnumerateStreamsComplete);
    // Link DeleteReplay() delegate to function
    //OnDeleteFinishedStreamCompleteDelegate = FOnDeleteFinishedStreamComplete::CreateUObject(this, &UMyGameInstance::OnDeleteFinishedStreamComplete);
}

void UReplayGameInstance::StartRecordingReplay(const FString& ReplayName)
{
    FString FriendlyName = FString::Printf(TEXT("Replay-%d-%d-%d"), FDateTime::Now().GetYear(), FDateTime::Now().GetMonth(), FDateTime::Now().GetDay());
    Super::StartRecordingReplay(ReplayName, FriendlyName);
}

void UReplayGameInstance::StopRecordingReplay()
{
    Super::StopRecordingReplay();
}

void UReplayGameInstance::PlayReplay(const FString& ReplayName)
{
    Super::PlayReplay(ReplayName);
}

void UReplayGameInstance::PauseReplay()
{
    if (IsReplayActive() && GetWorld()->GetDemoNetDriver())
    {
        GetWorld()->GetWorldSettings()->DemoPlayTimeDilation = 0;
    }
}

void UReplayGameInstance::ResumeReplay()
{
    if (IsReplayActive() && GetWorld()->GetDemoNetDriver())
    {
        GetWorld()->GetWorldSettings()->DemoPlayTimeDilation = 1.f;
    }
}

bool UReplayGameInstance::RenameReplay(const FString& OldReplayName, const FString& NewReplayName)
{
    // Replace with actual rename logic; depends on your file storage setup
    return IFileManager::Get().Move(*NewReplayName, *OldReplayName); // Example placeholder
}

TArray<FString> UReplayGameInstance::GetSavedReplays()
{
    TArray<FString> ReplayList;
    IFileManager& FileManager = IFileManager::Get();
    FString ReplayFolder = FPaths::ProjectSavedDir() / TEXT("Demos");

    FileManager.FindFiles(ReplayList, *ReplayFolder, TEXT(".replay"));

    return ReplayList;
}

void UReplayGameInstance::JumpToReplayTime(float TimeInSeconds)
{
    if (IsReplayActive() && GetWorld()->GetDemoNetDriver())
    {
        GetWorld()->GetDemoNetDriver()->GotoTimeInSeconds(TimeInSeconds);
    }
}

float UReplayGameInstance::GetCurrentReplayTime() const
{
    if (IsReplayActive() && GetWorld()->GetDemoNetDriver())
    {
        return GetWorld()->GetDemoNetDriver()->GetDemoCurrentTime();
    }
    return 0.0f;
}

float UReplayGameInstance::GetTotalReplayTime() const
{
    if (IsReplayActive() && GetWorld()->GetDemoNetDriver())
    {
        return GetWorld()->GetDemoNetDriver()->GetDemoTotalTime();
    }
    return 0.0f;
}

bool UReplayGameInstance::IsReplayActive() const
{
    return GetWorld() && GetWorld()->GetDemoNetDriver() && GetWorld()->GetDemoNetDriver()->IsPlaying();
}

void UReplayGameInstance::FindReplays()
{
    if (EnumerateStreamsPtr.Get())
    {
        EnumerateStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(), int32(), FString(), FJsonSerializableArray(), OnEnumerateStreamsCompleteDelegate);
    }
}

void UReplayGameInstance::OnEnumerateStreamsComplete(const FEnumerateStreamsResult& AllReplays)
{
    m_AllReplays.Empty();
    for (FNetworkReplayStreamInfo StreamInfo : AllReplays.FoundStreams)
    {
        if (!StreamInfo.bIsLive)
        {
            m_AllReplays.Add(FS_ReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
        }
    }

    OnReplyFound.Broadcast(m_AllReplays);
}
