// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "SProceduralRoom.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API ASProceduralRoom : public ALevelScriptActor
{
	GENERATED_BODY()
	
public :
	FTransform GetExitTransform() {
		if (ExitTransform)
			return ExitTransform->GetActorTransform();
		else
			return FTransform();
	}

	FTransform GetEntryTransform() {
		if (EntryTransform)
			return EntryTransform->GetActorTransform();
		else
			return FTransform();
	}

private : 
	UPROPERTY(EditAnywhere)
	AActor* EntryTransform;
	
	UPROPERTY(EditAnywhere)
	AActor* ExitTransform;

};
