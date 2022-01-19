// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./ProceduralLevels/SRoomTemplate.h"
//#include "./ProceduralLevels/SLevelIsland.h"

#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameManager.generated.h"


USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()

public:
	FTile(TileType Type = TileType::PT_LEVELROOM, int LevelRoomID, const FName& Name = "")
		: Type(Type), LevelName(Name), LevelRoomID(LevelRoomID) {}

	TileType Type;
	FName LevelName;

	// ID of TilesPool[Type] 
	int LevelRoomID;
};

//USTRUCT(BlueprintType)
//struct FRoom : public FTile {
//	GENERATED_BODY()
//
//public:
//	bool IsFinished;
//
//	FRoomInLevel(int LevelRoomID = -1, bool isFinished = false)
//		: FTile(TileType::LEVELROOM, ""), LevelRoomID(LevelRoomID), IsFinished(isFinished) {}
//};

USTRUCT(BlueprintType)
struct FGrid
{
	GENERATED_BODY()

public:
	FGrid();

	// RoomLevelPathID : Start = 0 --> End = ROOM_COUNT
	// PoolTileID      : ID of TilesPool[TileType::PT_LEVELROOM] 
	void SetRoomLevelTile(int RoomLevelPathID, int PoolTileID)
	{
		// Start Room is always on the same coordinates (2,2)
		if (RoomLevelPathID == 0)
		{
			int RoomIndex = (GetGridWidth() * 2) + 2;
			Grid[RoomIndex] = FTile(TileType::PT_LEVELROOM, PoolTileID, "");
			return;
		}
		// Second Room is always on the same coordinates (4,2)
		if (RoomLevelPathID == 1)
		{
			int RoomIndex = (GetGridWidth() * 2) + 4;
			Grid[RoomIndex] = FTile(TileType::PT_LEVELROOM, PoolTileID, "");
			return;
		}

		// For other tiles, compute coordinates with the help of previous one
		
		int PreviousTileIndex = ;
		int IndexTile = PreviousTileIndex;
		Grid[IndexTile] = FTile(TileType::PT_LEVELROOM, PoolTileID, "");

	}

	int GetGridWidth() const { return (ROOM_COUNT * 2) + 1; }

	// Number of rooms in a level island (including start and end rooms)
	const int ROOM_COUNT = 6;

	TArray<FTile> Grid;
};

USTRUCT(BlueprintType)
struct FIslandLevel {
	GENERATED_BODY()

public:
	FIslandLevel() {};
	FIslandLevel(FVector worldPosition, BiomeType biome, bool isMaritime, bool isFinished = false)
		: Rooms({}), WorldPosition(worldPosition), Biome(BiomeType::FOREST), IsFinished(isFinished), IsMaritime(isMaritime) {}
	// FVector GetWorldPosition() const { return WorldPosition; }

	//TArray<FRoomInLevel> Rooms;
	FVector WorldPosition;

	TArray<bool> FinishedStates;

	BiomeType Biome;
	bool IsFinished = false;
	bool IsMaritime;

	FGrid Grid;
};






UCLASS()
class PROTO_BOAT_API UGameManager : public UGameInstance
{
	GENERATED_BODY()
public:
	UGameManager();
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void InitializeTilesPool();

	//UFUNCTION(BlueprintCallable, Category = "Custom")
	//bool AreLevelsShowned();
	//
	//UFUNCTION(BlueprintCallable, Category = "Custom")
	//void InitializeRoomsPool();

	//UPROPERTY(EditAnywhere,BlueprintReadOnly)
	//TArray<FSRoomTemplate> PoolOfRoom;

	// TODO remove pointers
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FIslandLevel> Islands;

	// All levels to load for islands
	//TArray< ULevelStreaming* > StreamingLevels;

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
	TileType FindTileTypeFromLevelName(const FName& LevelName) const;

	void InitializeIslandLevel(FIslandLevel& level);

	int GetGridWidth() const { return (ROOM_COUNT * 2) + 1; }

private:
	bool PoolInitialized = false;

	TMap< TileType, TArray<FTile> > TilesPool;

	// Number of rooms in a level island (including start and end rooms)
	const int ROOM_COUNT = 6;
	
};

