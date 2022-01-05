// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./SRoomInLevel.h"
#include "../Utility.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SIslandLevel.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API USIslandLevel : public UObject
{
	GENERATED_BODY()
private :
	TArray<USRoomInLevel> Rooms;
	FVector WorldPosition;

	BiomeType Biome;
	bool IsFinished;
};
