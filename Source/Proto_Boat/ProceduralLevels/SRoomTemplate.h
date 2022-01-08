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
USTRUCT(BlueprintType)
struct FSRoomTemplate
{
	GENERATED_BODY()
public:
	FSRoomTemplate() {};
	FSRoomTemplate(FName _SubLevelName, FVector _ExitPosition) : SubLevelName(_SubLevelName), ExitPosition(_ExitPosition){};

//private :
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SubLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ExitPosition;
	FVector EntryPosition;

	BiomeType GetBiomeType ();
	RoomType GetRoomType ();
	int GetId ();

	TArray<FString> getTabName();
};
