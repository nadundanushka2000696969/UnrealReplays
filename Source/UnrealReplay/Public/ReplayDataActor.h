#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplayDataActor.generated.h"

USTRUCT(BlueprintType)
struct FReplayFrameData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float PlayerEnemyDistance;

    UPROPERTY(BlueprintReadOnly)
    FRotator PlayerCameraRotation;

    UPROPERTY(BlueprintReadOnly)
    float CameraRotationSpeed;

    UPROPERTY(BlueprintReadOnly)
    float PlayerSpeed;

    UPROPERTY(BlueprintReadOnly)
    bool Fear;
};

UCLASS()
class UNREALREPLAY_API AReplayDataActor : public AActor
{
    GENERATED_BODY()

public:
    AReplayDataActor();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Save recorded data to a file
    UFUNCTION(BlueprintCallable, Category = "Replay")
    void SaveRecordedDataToFile(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "Replay")
    void SetEnemy(ACharacter* InEnemy)
    {
        Enemy = InEnemy;
    }

    UFUNCTION(BlueprintCallable, Category = "ReplayActor")
    void SetFear(bool fear)
    {
        Fear = fear;
    }

    UPROPERTY(BlueprintReadOnly)
    bool Fear = false;
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    void RecordFrameData(float DeltaTime);
    UDemoNetDriver* DemoDriver = nullptr;
    ACharacter* Player = nullptr;
    ACharacter* Enemy = nullptr;


    bool bIsRecording = false;
    bool bWasReplayPlaying = false;
    bool bSavedToAFile = false;

    TArray<FReplayFrameData> RecordedData;

    FRotator PreviousCameraRotation = FRotator::ZeroRotator;

    void HandleReplayStart();
    void HandleReplayEnd();
};
