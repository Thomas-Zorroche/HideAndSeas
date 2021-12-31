// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProceduralRoom.h"
#include "SStartRoom.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API ASStartRoom : public ASProceduralRoom
{
	GENERATED_BODY()

protected :
	bool IsCompleted = true;
};
