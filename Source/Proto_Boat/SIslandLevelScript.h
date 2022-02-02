// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Interfaces/SLevelLoaded.h"
#include "SIslandLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API ASIslandLevelScript : public ALevelScriptActor, public ISLevelLoaded
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Broadcast")
	void OnLevelLoaded();  // This is the prototype declared in the interface
	virtual void OnLevelLoaded_Implementation() override; // This is the declaration of the implementation
	
protected:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SPlayerStart")
	void OnLevelReady();

};
