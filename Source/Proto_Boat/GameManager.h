// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./ProceduralLevels/SRoomTemplate.h"
#include "./ProceduralLevels/SIslandLevel.h"

#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameManager.generated.h"

UCLASS()
class PROTO_BOAT_API UGameManager : public UGameInstance
{
	GENERATED_BODY()
public:
	UGameManager();
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void GetStreamingLevels();

	UFUNCTION(BlueprintCallable, Category = "Custom")
	bool AreLevelsShowned();

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void InitializeRoomsPool();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FSRoomTemplate> PoolOfRoom;

	// TODO remove pointers
	UPROPERTY(EditAnywhere)
	TArray<USIslandLevel*> Islands;

	TArray< ULevelStreaming* > StreamingLevels;

	const TArray<USIslandLevel*>& GetIslandLevels() const { return Islands; }
	bool HasIslandLevels() const { return Islands.Num() > 0; }


	// Return index
	//int AddIslandLevel(FVector, biome, isMaritime);


	void GenerateIslands(TArray<FVector> IslandLocations)
	{
		
	}

private:
	bool PoolInitialized = false;
	
};
