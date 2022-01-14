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
class USIslandLevel : public UObject
{
	GENERATED_BODY()

public:
	void Init(FVector worldPosition, BiomeType biome, bool isMaritime);
	FVector GetWorldPosition() const { return WorldPosition; }

private :
	TArray<USRoomInLevel> Rooms;
	FVector WorldPosition;

	BiomeType Biome;
	bool IsFinished = false;
	bool IsMaritime;

	static int const LEVELSIZE = 6;
};
