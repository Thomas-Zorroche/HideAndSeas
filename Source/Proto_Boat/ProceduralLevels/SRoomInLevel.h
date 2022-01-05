// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SRoomInLevel.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API USRoomInLevel : public UObject
{
	GENERATED_BODY()
private:
	int PoolIndex;

	FVector WorldPosition;
	bool IsFinished;
};
