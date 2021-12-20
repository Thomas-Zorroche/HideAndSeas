// Fill out your copyright notice in the Description page of Project Settings.


#include "SProceduralLevel.h"
#include "SProceduralRoom.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ASProceduralLevel::ASProceduralLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Rooms.Add(RoomType::START, {});
    Rooms.Add(RoomType::END, {});
    Rooms.Add(RoomType::BACKTOFRONT, {});
    Rooms.Add(RoomType::RIGHTTOLEFT, {});
    Rooms.Add(RoomType::LEFTTOBACK, {});
    Rooms.Add(RoomType::FRONTTORIGHT, {});
}

void ASProceduralLevel::SortAllLevel() {

    ProceduralWorld = GetWorld();

    const TArray<ULevelStreaming*>& streamedLevels = ProceduralWorld->GetStreamingLevels();
    for (ULevelStreaming* streamingLevel : streamedLevels)
    {
        // Sort all sub Levels
        FString levelName = streamingLevel->GetWorldAssetPackageName();
        if (levelName.Contains(TEXT("Start"))) 
            Rooms.Find(RoomType::START)->Rooms.Add(streamingLevel);
        else if (levelName.Contains(TEXT("End"))) 
            Rooms.Find(RoomType::END)->Rooms.Add(streamingLevel);
        else if (levelName.Contains(TEXT("FtB")))
            Rooms.Find(RoomType::BACKTOFRONT)->Rooms.Add(streamingLevel);
        else if (levelName.Contains(TEXT("LtR"))) 
            Rooms.Find(RoomType::RIGHTTOLEFT)->Rooms.Add(streamingLevel);
        else if (levelName.Contains(TEXT("LtB")))
            Rooms.Find(RoomType::LEFTTOBACK)->Rooms.Add(streamingLevel);
        else if (levelName.Contains(TEXT("FtR")))
            Rooms.Find(RoomType::FRONTTORIGHT)->Rooms.Add(streamingLevel);


        UE_LOG(LogTemp, Warning, TEXT("LEVEL NAME : %s"), *levelName);
    }
}

// Called when the game starts or when spawned
void ASProceduralLevel::BeginPlay()
{
	Super::BeginPlay();



    // Load start level 
    //if (Rooms.Find(RoomType::START)) {
    //    ULevelStreaming* level = Rooms.Find(RoomType::START)->Rooms[0];

    //    CreateLevelAtPosition(level, FTransform());

    //    CurrentLevel = level;
    //    level->OnLevelShown.AddDynamic(this, &ASProceduralLevel::OnCurrentLevelShown);

        //auto levelActor = ProceduralWorld->GetLevelScriptActor(level->GetLoadedLevel());
        //auto levelRoom = Cast<ASProceduralRoom>(levelActor);

        ////auto script = level->GetLevelScriptActor();
        //if (levelRoom) {
        //    UE_LOG(LogTemp, Warning, TEXT("omg"));
        //}
        //else
        //{
        //    UE_LOG(LogTemp, Warning, TEXT("ptn"));
        //}

        //UGameplayStatics::LoadStreamLevel(this, *levelName, true, true, FLatentActionInfo());
    //}

}

// Called every frame
void ASProceduralLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform ASProceduralLevel::GetExitTransform(ULevelStreaming* level){
    ASProceduralRoom* room = Cast<ASProceduralRoom>(level->GetLevelScriptActor());

    if (room) {
        UE_LOG(LogTemp, Warning, TEXT("Cast succeed"));
        return room->GetExitTransform();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Cast failed"));
        return FTransform();
    }

}

void ASProceduralLevel::OnCurrentLevelShown() {

    if (Rooms.Find(RoomType::RIGHTTOLEFT)) {
        ULevelStreaming* level = Rooms.Find(RoomType::RIGHTTOLEFT)->Rooms[0];

        CreateLevelAtPosition(level, NextSpawn);
    }
}

void ASProceduralLevel::CreateLevelAtPosition(ULevelStreaming* level, FTransform transform) {
    level->CreateInstance(TEXT("SecondLevel"));
    level->LevelTransform = transform;
    level->SetShouldBeLoaded(true);
    level->SetShouldBeVisible(true);
}