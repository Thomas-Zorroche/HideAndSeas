// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./ProceduralLevels/SRoomTemplate.h"
//#include "./ProceduralLevels/SLevelIsland.h"

#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameManager.generated.h"

USTRUCT(BlueprintType)
struct FRoomInLevel {
	GENERATED_BODY()

public:
	// Index in GameManager PoolOfRoom, corresponding to the RoomTemplate to spawn
	int PoolIndex;

	FVector WorldPosition;
	bool IsFinished;
	FRoomInLevel() {};
	FRoomInLevel(int poolIndex, FVector position, bool isFinished = false)
		: PoolIndex(poolIndex), WorldPosition(position), IsFinished(isFinished) {}
};

USTRUCT(BlueprintType)
struct FIslandLevel {
	GENERATED_BODY()

public:
	FIslandLevel() {};
	FIslandLevel(FVector worldPosition, BiomeType biome, bool isMaritime, bool isFinished = false)
		: Rooms({}), WorldPosition(worldPosition), Biome(BiomeType::FOREST), IsFinished(isFinished), IsMaritime(isMaritime) {}
	// FVector GetWorldPosition() const { return WorldPosition; }

	TArray<FRoomInLevel> Rooms;
	FVector WorldPosition;

	BiomeType Biome;
	bool IsFinished = false;
	bool IsMaritime;

	static int const LEVELSIZE = 6;
};


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

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FSRoomTemplate> PoolOfRoom;

	// TODO remove pointers
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FIslandLevel> Islands;

	TArray< ULevelStreaming* > StreamingLevels;

	const TArray<FIslandLevel>& GetIslandLevels() const { return Islands; }
	
	UFUNCTION(BlueprintCallable, Category = "Custom")
	bool HasIslandLevels() const { return Islands.Num() > 0; }


	// Return index
	//int AddIslandLevel(FVector, biome, isMaritime);

	static BiomeType GetRandomBiomeType() {
		return  static_cast<BiomeType>(FMath::RandRange(0, (int)BiomeType::MAX - 1));
	}

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void GenerateIslands(TArray<FVector> IslandLocations, bool IsMaritime);

	int GetRandomRoom(RoomType roomType, BiomeType biome);

	TArray<int> FilterByBiomeAndType(BiomeType biome, RoomType roomType) const;

private:
	void InitializeIslandLevel(FIslandLevel& level);

	bool PoolInitialized = false;
	
};

