// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility.generated.h"

/**
 * 
 */


// TODO move this in other file

UENUM(BlueprintType)
enum class TileType : uint8
{
	PT_TRANSITION = 0	UMETA(DisplayName = "Playable Tile Transition"),
	PT_LEVELROOM = 1	UMETA(DisplayName = "Playable Tile Level Room"),
	NPT_LANDSCAPE = 2	UMETA(DisplayName = "Non Playable Tile Landscape"),
	NPT_TRANSITION = 3	UMETA(DisplayName = "Non Playable Tile Transition"),
	NPT_SQUARE = 4		UMETA(DisplayName = "Non Playable Tile Square"),
	MAX = 5				UMETA(DisplayName = "Do not use (Max Value)"),
};

UENUM(BlueprintType)
enum class RoomType : uint8
{
	START_Y = 0			UMETA(DisplayName = "Start (Y Axis)"),
	START_X = 1			UMETA(DisplayName = "Start (X Axis)"),
	END_Y = 2			UMETA(DisplayName = "End (Y Axis)"),
	END_X = 3			UMETA(DisplayName = "End (X Axis)"),
	LEFTTOBACK = 4		UMETA(DisplayName = "Left to Back"),
	FRONTTORIGHT = 5	UMETA(DisplayName = "Front to Right"),
	RIGHTTOLEFT = 6		UMETA(DisplayName = "Right to Left"),
	BACKTOFRONT = 7		UMETA(DisplayName = "Back to Front"),
	MAX = 8				UMETA(DisplayName = "Do not use (Max Value)"),
};

inline FString GetRoomTypeStr(RoomType Room)
{
	switch (Room)
	{
	case RoomType::START_Y:		 return "StartY"; break;
	case RoomType::START_X:		 return "StartX"; break;
	case RoomType::END_Y:		 return "EndY"; break;
	case RoomType::END_X:		 return "EndX"; break;
	case RoomType::LEFTTOBACK:   return "LtB"; break;
	case RoomType::FRONTTORIGHT: return "FtR";  break;
	case RoomType::RIGHTTOLEFT:  return "LtR";  break;
	case RoomType::BACKTOFRONT:  return "FtB";  break;
	default:					 return "";  break;
	}
}


UENUM(BlueprintType)
enum class BiomeType : uint8
{
	FOREST = 0			UMETA(DisplayName = "Forest"),
	VILLAGE = 1			UMETA(DisplayName = "Village"),
	CASTLE = 2			UMETA(DisplayName = "Castle"),
	MAX = 3				UMETA(DisplayName = "Do not use (Max Value)"),
};

inline FString GetBiomeTypeStr(BiomeType Biome)
{
	switch (Biome)
	{
	case BiomeType::FOREST:  return "Forest"; break;
	case BiomeType::VILLAGE: return "Village"; break;
	case BiomeType::CASTLE:  return "Castle"; break;
	default:				 return "";  break;
	}
}

inline RoomType GetRandomRoomType(RoomType previousRoomType) {
	TArray<RoomType> choice = {};

	if (previousRoomType == RoomType::START_Y || previousRoomType == RoomType::RIGHTTOLEFT || previousRoomType == RoomType::FRONTTORIGHT) {
		choice.Add(RoomType::RIGHTTOLEFT);
		choice.Add(RoomType::LEFTTOBACK);
	}
	else {
		choice.Add(RoomType::BACKTOFRONT);
		choice.Add(RoomType::FRONTTORIGHT);
	}

	return choice[FMath::RandRange(0, 1)];
}

inline RoomType GetRandomEndType(RoomType previousRoomType) {
	if (previousRoomType == RoomType::START_Y || previousRoomType == RoomType::RIGHTTOLEFT || previousRoomType == RoomType::FRONTTORIGHT)
		return RoomType::END_Y;
	else return RoomType::END_X;
}

inline bool IsExitOnYAxis(RoomType roomType) {
	if (roomType == RoomType::RIGHTTOLEFT || roomType == RoomType::FRONTTORIGHT)
		return true;
	return false;
}