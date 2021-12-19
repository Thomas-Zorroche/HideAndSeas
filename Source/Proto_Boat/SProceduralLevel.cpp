// Fill out your copyright notice in the Description page of Project Settings.


#include "SProceduralLevel.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ASProceduralLevel::ASProceduralLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ProceduralWorld = GetWorld();

    Rooms.Add(RoomType::START, {});
    Rooms.Add(RoomType::END, {});
    Rooms.Add(RoomType::BACKTOFRONT, {});
    Rooms.Add(RoomType::RIGHTTOLEFT, {});
    Rooms.Add(RoomType::LEFTTOBACK, {});
    Rooms.Add(RoomType::FRONTTORIGHT, {});


}

// Called when the game starts or when spawned
void ASProceduralLevel::BeginPlay()
{

    const TArray<ULevelStreaming*>& streamedLevels = ProceduralWorld->GetStreamingLevels();
    for (ULevelStreaming* streamingLevel : streamedLevels)
    {
        // Sort all sub Levels
        FString levelName = streamingLevel->GetWorldAssetPackageName();
        if (levelName.Contains(TEXT("Start"))) 
            Rooms.Find(RoomType::START)->RoomsName.Add(streamingLevel->GetWorldAssetPackageName());
        else if (levelName.Contains(TEXT("End"))) 
            Rooms.Find(RoomType::END)->RoomsName.Add(streamingLevel->GetWorldAssetPackageName());
        else if (levelName.Contains(TEXT("FtB")))
            Rooms.Find(RoomType::BACKTOFRONT)->RoomsName.Add(streamingLevel->GetWorldAssetPackageName());
        else if (levelName.Contains(TEXT("LtR"))) 
            Rooms.Find(RoomType::RIGHTTOLEFT)->RoomsName.Add(streamingLevel->GetWorldAssetPackageName());
        else if (levelName.Contains(TEXT("LtB")))
            Rooms.Find(RoomType::LEFTTOBACK)->RoomsName.Add(streamingLevel->GetWorldAssetPackageName());
        else if (levelName.Contains(TEXT("FtR")))
            Rooms.Find(RoomType::FRONTTORIGHT)->RoomsName.Add(streamingLevel->GetWorldAssetPackageName());


        UE_LOG(LogTemp, Warning, TEXT("LEVEL NAME : %s"), *levelName);
    }

    // Load start level 
    if (Rooms.Find(RoomType::START)) {
        FString levelName = Rooms.Find(RoomType::START)->RoomsName[0];
        UE_LOG(LogTemp, Warning, TEXT("START LVL : %s"), *levelName);
        UGameplayStatics::LoadStreamLevel(this, *levelName, true, true, FLatentActionInfo());
    }

	Super::BeginPlay();
}

// Called every frame
void ASProceduralLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

