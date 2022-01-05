// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Utility.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SRoomTemplate.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API USRoomTemplate : public UObject
{
	GENERATED_BODY()
private :
	FName SubLevelName;

	FVector ExitPosition;
	FVector EntryPosition;

	BiomeType GetBiomeType = BiomeType::FOREST;
	RoomType GetRoomType = RoomType::START;
	int GetId = 0;
};
