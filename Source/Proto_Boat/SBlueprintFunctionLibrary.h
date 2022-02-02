// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/LevelStreaming.h" 
#include "SBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API USBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "UtilsLibrary")
	static ULevelStreaming* GetStreamingLevelFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "UtilsLibrary")
	static AActor* MoveActorToLevelStreaming(AActor* ActorToMove, ULevelStreaming* StreamingLevel);
	
};
