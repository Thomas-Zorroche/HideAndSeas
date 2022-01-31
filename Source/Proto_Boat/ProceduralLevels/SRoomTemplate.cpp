// Fill out your copyright notice in the Description page of Project Settings.


#include "./SRoomTemplate.h"
//
//BiomeType FSRoomTemplate::GetBiomeType() const
//{
//    TArray<FString> tabName = getTabName();
//    check(tabName.Num() > 3);
//    FString biomeTypeString = tabName[0];
//
//    if (biomeTypeString == "Forest")
//        return BiomeType::FOREST;
//    else if (biomeTypeString == "Village")
//        return BiomeType::VILLAGE;
//    else if (biomeTypeString == "Castle")
//        return BiomeType::CASTLE;
//    UE_LOG(LogTemp, Error, TEXT("Did not find Biome Type"));
//    return BiomeType::MAX;
//}
//
//RoomType FSRoomTemplate::GetRoomType() const 
//{
//    TArray<FString> tabName = getTabName();
//    check(tabName.Num() > 3);
//    FString roomTypeString = tabName[2];
//
//    if (roomTypeString == "Start")
//        return RoomType::START;
//    else if (roomTypeString == "End")
//        return RoomType::END;
//    else if (roomTypeString == "FtB")
//        return RoomType::BACKTOFRONT;
//    else if (roomTypeString == "LtR")
//        return RoomType::RIGHTTOLEFT;
//    else if (roomTypeString == "LtB")
//        return RoomType::LEFTTOBACK;
//    else if (roomTypeString == "FtR")
//        return RoomType::FRONTTORIGHT;
//    UE_LOG(LogTemp, Error, TEXT("Did not find Room Type"));
//    return RoomType::MAX;
//}
//
//int FSRoomTemplate::GetId()
//{
//    TArray<FString> tabName = getTabName();
//    check(tabName.Num() > 3);
//    return FCString::Atoi(*tabName[2]);
//}
//
//TArray<FString> FSRoomTemplate::getTabName() const
//{
//    TArray<FString> parsed;
//    SubLevelName.ToString().ParseIntoArray(parsed, TEXT("_"), false);
//    // Parsed now contains ["BiomeType", "RoomType", "id",]
//    check(parsed.Num() > 3);
//    return parsed;
//}
