// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Utility.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Misc/AssertionMacros.h"
#include "Kismet/KismetStringLibrary.h"
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

	BiomeType GetBiomeType ();
	RoomType GetRoomType ();
	int GetId ();

	TArray<FString> getTabName();
};
