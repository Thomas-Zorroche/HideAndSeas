// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utility.generated.h"

/**
 * 
 */


// TODO move this in other file

UENUM(BlueprintType)
enum class RoomType : uint8
{
	START = 0			UMETA(DisplayName = "Start"),
	END = 1				UMETA(DisplayName = "End"),
	LEFTTOBACK = 2		UMETA(DisplayName = "Left to Back"),
	FRONTTORIGHT = 3	UMETA(DisplayName = "Front to Right"),
	RIGHTTOLEFT = 4		UMETA(DisplayName = "Right to Left"),
	BACKTOFRONT = 5		UMETA(DisplayName = "Back to Front"),
	MAX = 6				UMETA(DisplayName = "Do not use (Max Value)"),
};

UENUM(BlueprintType)
enum class BiomeType : uint8
{
	FOREST = 0			UMETA(DisplayName = "Forest"),
	VILLAGE = 1			UMETA(DisplayName = "Village"),
	CASTLE = 2			UMETA(DisplayName = "Castle"),
	MAX = 3				UMETA(DisplayName = "Do not use (Max Value)"),
};

inline RoomType GetRandomRoomType(RoomType previousRoomType) {
	TArray<RoomType> choice = {};

	if (previousRoomType == RoomType::START || previousRoomType == RoomType::RIGHTTOLEFT || previousRoomType == RoomType::FRONTTORIGHT) {
		choice.Add(RoomType::RIGHTTOLEFT);
		choice.Add(RoomType::LEFTTOBACK);
	}
	else {
		choice.Add(RoomType::BACKTOFRONT);
		choice.Add(RoomType::FRONTTORIGHT);
	}

	return choice[FMath::RandRange(0, 1)];
}

inline bool IsExitOnYAxis(RoomType roomType) {
	if (roomType == RoomType::RIGHTTOLEFT || roomType == RoomType::FRONTTORIGHT)
		return true;
	return false;
}