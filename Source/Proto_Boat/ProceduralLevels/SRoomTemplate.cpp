// Fill out your copyright notice in the Description page of Project Settings.


#include "./SRoomTemplate.h"

BiomeType USRoomTemplate::GetBiomeType()
{
    if (SubLevelName.ToString().Contains(TEXT("Forest")))
        return BiomeType::FOREST;
    else if (SubLevelName.ToString().Contains(TEXT("Village")))
        return BiomeType::VILLAGE;
    else if (SubLevelName.ToString().Contains(TEXT("Castle")))
        return BiomeType::CASTLE;
    UE_LOG(LogTemp, Error, TEXT("Did not find Biome Type"));
    return BiomeType::MAX;
}

RoomType USRoomTemplate::GetRoomType() {
    /*TArray<FString> tabName;
    check(tabName.Num() > 2);
    switch (tabName[1])
    {
    default:
        break;
    }*/
    if (SubLevelName.ToString().Contains(TEXT("Start")))
       return RoomType::START;
    else if (SubLevelName.ToString().Contains(TEXT("End")))
        return RoomType::END;
    else if (SubLevelName.ToString().Contains(TEXT("FtB")))
        return RoomType::BACKTOFRONT;
    else if (SubLevelName.ToString().Contains(TEXT("LtR")))
        return RoomType::RIGHTTOLEFT;
    else if (SubLevelName.ToString().Contains(TEXT("LtB")))
        return RoomType::LEFTTOBACK;
    else if (SubLevelName.ToString().Contains(TEXT("FtR")))
        return RoomType::FRONTTORIGHT;
    UE_LOG(LogTemp, Error, TEXT("Did not find Room Type"));
    return RoomType::MAX;
}

int USRoomTemplate::GetId()
{
    TArray<FString> tabName;
    check(tabName.Num()>3);
    return FCString::Atoi(*tabName[2]);
}

TArray<FString> getTabName(FName name) {
    TArray<FString> parsed;
    name.ToString().ParseIntoArray(parsed, TEXT("_"), false);
    // Parsed now contains ["BiomeType", "RoomType", "id",]
    return parsed;
}
