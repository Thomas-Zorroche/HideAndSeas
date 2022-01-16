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

}

// Called every frame
void ASProceduralLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform ASProceduralLevel::GetExitTransform(ULevelStreaming* level){
    ASProceduralRoom* room = Cast<ASProceduralRoom>(level->GetLevelScriptActor());
    if (room) {
        return room->GetExitTransform();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Cannot get Exit Transform"));
        return FTransform();
    }
}

//RoomType ASProceduralLevel::GetRandomRoomType(RoomType previousRoomType) {
//    TArray<RoomType> choice;
//
//    if (previousRoomType == RoomType::START || previousRoomType == RoomType::RIGHTTOLEFT || previousRoomType == RoomType::FRONTTORIGHT) {
//        choice.Add(RoomType::RIGHTTOLEFT);
//        choice.Add(RoomType::LEFTTOBACK);
//    }
//    else {
//        choice.Add(RoomType::BACKTOFRONT);
//        choice.Add(RoomType::FRONTTORIGHT);
//    }
// 
//    return choice[FMath::RandRange(0, 1)];
//}

//bool ASProceduralLevel::IsExitOnYAxis(RoomType roomType) {
//    if (roomType == RoomType::RIGHTTOLEFT || roomType == RoomType::FRONTTORIGHT)
//        return true;
//    return false;
//}

//void ASProceduralLevel::OnCurrentLevelShown() {
//
//    if (Rooms.Find(RoomType::RIGHTTOLEFT)) {
//        ULevelStreaming* level = Rooms.Find(RoomType::RIGHTTOLEFT)->Rooms[0];
//
//        CreateLevelAtPosition(level, NextSpawn);
//    }
//}

//void ASProceduralLevel::CreateLevelAtPosition(ULevelStreaming* level, FTransform transform) {
//    level->CreateInstance(TEXT("SecondLevel"));
//    level->LevelTransform = transform;
//    level->SetShouldBeLoaded(true);
//    level->SetShouldBeVisible(true);
//}