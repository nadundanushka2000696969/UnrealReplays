#include "ReplayDataActor.h"
#include "Engine/DemoNetDriver.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputCoreTypes.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

// Sets default values
AReplayDataActor::AReplayDataActor()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AReplayDataActor::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);
    for (const AActor* Character : FoundActors)
    {
        if (Character->ActorHasTag(FName("Player")))
        {
            Player = (ACharacter*)Character;
        }
        else
        {
            Enemy = (ACharacter*)Character;
        }
    }

    DemoDriver = GetWorld()->GetDemoNetDriver();
}

// Called every frame
void AReplayDataActor::Tick(float DeltaTime)
{
    if (DemoDriver->IsRecording())
    {
        return;
    }

    Super::Tick(DeltaTime);

    float TotalTime = 0.f;
    float CurrentTime = 0.f;
    bool bIsCurrentTimeExceeding = false;
    if (GetWorld())
    {
        if (DemoDriver)
        {
            TotalTime = DemoDriver->GetDemoTotalTime();
            CurrentTime = DemoDriver->GetDemoCurrentTime();
            bIsCurrentTimeExceeding = CurrentTime > TotalTime;

            FString Message = FString::Printf(TEXT("TotalTime: %.2f, CurrentTime: %.2f, Exceeding: %s"),
                TotalTime, CurrentTime, bIsCurrentTimeExceeding ? TEXT("True") : TEXT("False"));
            if(GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, Message);
        }
        else
        {
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, TEXT("DemoNetDriver is not valid."));
        }
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, TEXT("World is null."));
    }

    if (!bIsCurrentTimeExceeding)
    {
        if (!bIsRecording)
        {
            HandleReplayStart();
        }
        RecordFrameData(DeltaTime);
    }
    else if (!bSavedToAFile)
    {
        HandleReplayEnd();
    }

    bWasReplayPlaying = bIsCurrentTimeExceeding;
}


void AReplayDataActor::RecordFrameData(float DeltaTime)
{
    if (!GetWorld()) return;

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController) return;
    
    FVector PlayerLocation = FVector::ZeroVector;
    FVector PlayerVelocity = FVector::ZeroVector;
    FRotator CameraRotation = FRotator::ZeroRotator;
    FVector EnemyLocation = FVector::ZeroVector;
    float PlayerEnemyDistance = 0.f;

    if(Player)
    {
        PlayerLocation = Player->GetActorLocation();
        PlayerVelocity = Player->GetVelocity();
        CameraRotation = Player->GetComponentByClass<UCameraComponent>()->GetComponentRotation();
    }

    if(Enemy)
    {
        EnemyLocation = Enemy->GetActorLocation();
        PlayerEnemyDistance = FVector::Dist(PlayerLocation, EnemyLocation);
    }

    float CameraRotationSpeed = 0.0f;
    if (PreviousCameraRotation != FRotator::ZeroRotator)
    {
        FRotator DeltaRotation = CameraRotation - PreviousCameraRotation;
        CameraRotationSpeed = FVector(
            DeltaRotation.Pitch / DeltaTime,
            DeltaRotation.Yaw / DeltaTime,
            DeltaRotation.Roll / DeltaTime
        ).Size();
    }
    PreviousCameraRotation = CameraRotation;

    float PlayerSpeed = PlayerVelocity.Size();

    FReplayFrameData FrameData;
    FrameData.PlayerEnemyDistance = PlayerEnemyDistance;
    FrameData.PlayerCameraRotation = CameraRotation;
    FrameData.PlayerSpeed = PlayerSpeed;
    FrameData.CameraRotationSpeed = CameraRotationSpeed;
    FrameData.Fear = Fear;
    RecordedData.Add(FrameData);
}

void AReplayDataActor::HandleReplayStart()
{
    bIsRecording = true;
    PreviousCameraRotation = FRotator::ZeroRotator;
    UE_LOG(LogTemp, Log, TEXT("Replay started. Recording data."));
}

void AReplayDataActor::HandleReplayEnd()
{
    bIsRecording = false;
    FString FilePath = FPaths::ProjectSavedDir() / "DemoExtractedData" / DemoDriver->GetActiveReplayName() + ".json";
    SaveRecordedDataToFile(FilePath);
    UE_LOG(LogTemp, Log, TEXT("Replay ended. Data saved to file."));
}

void AReplayDataActor::SaveRecordedDataToFile(const FString& FilePath)
{
    TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> ReplayDataArray;

    for (const FReplayFrameData& Frame : RecordedData)
    {
        TSharedRef<FJsonObject> FrameObject = MakeShareable(new FJsonObject);
        FrameObject->SetNumberField(TEXT("PlayerEnemyDistance"), Frame.PlayerEnemyDistance);
        FrameObject->SetStringField(TEXT("PlayerCameraRotation"), Frame.PlayerCameraRotation.ToString());
        FrameObject->SetNumberField(TEXT("PlayerSpeed"), Frame.PlayerSpeed);
        FrameObject->SetNumberField(TEXT("CameraRotationSpeed"), Frame.CameraRotationSpeed);
        FrameObject->SetBoolField(TEXT("Fear"), Frame.Fear);
        ReplayDataArray.Add(MakeShareable(new FJsonValueObject(FrameObject)));
    }

    JsonObject->SetArrayField(TEXT("ReplayData"), ReplayDataArray);

    FString JsonOutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonOutputString);
    if (FJsonSerializer::Serialize(JsonObject, Writer))
    {
        bSavedToAFile = FFileHelper::SaveStringToFile(JsonOutputString, *FilePath);
    }
}
