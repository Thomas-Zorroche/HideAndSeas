// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./ProceduralLevels/SRoomTemplate.h"
#include "./Utility.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Math/IntPoint.h" 
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SPatrolPath.h"
#include "GameManager.generated.h"


USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()

public:
	FTile(TileType Type = TileType::PT_LEVELROOM, FString Name = "", uint8 slID = 0)
		: Type(Type), LevelName(Name), StreamingLevelID(slID) {}

	TileType Type;
	FString LevelName;

	// ID in the GetWorld().StreamingLevels
	uint8 StreamingLevelID;

	// True first time the level streaming is shown. False after. 
	bool FirstTimeShown = true;

	TArray<ASPatrolPath*> PatrollerPaths;

	void FillPatrollerPaths(TArray<AActor*> Actors, const TArray<ULevelStreaming*>& StreamingLevels);

	void OnTileShown();
};


USTRUCT(BlueprintType)
struct FIslandLevel {
	GENERATED_BODY()

public:
	FIslandLevel() {};
	//  [TO DO] : Lorsqu'on aura assez de tiles changer la valeur du biome entrée en dur...
	FIslandLevel(FVector worldPosition, BiomeType biome, bool isMaritime, bool isFinished = false)
		:WorldPosition(worldPosition), Biome(BiomeType::FOREST /* biome */), IsFinished(isFinished), IsMaritime(isMaritime) {}
	// FVector GetWorldPosition() const { return WorldPosition; }

	//TArray<FRoomInLevel> Rooms;
	FVector WorldPosition;

	TArray<bool> FinishedStates;

	BiomeType Biome;
	bool IsFinished = false;
	bool IsMaritime;

	TArray<TArray<FTile>> Grid;

};


UCLASS()
class PROTO_BOAT_API UGameManager : public UGameInstance
{
	GENERATED_BODY()
public:
	UGameManager();

	/////////////////////////////////////////////////////////////////////////////////
	
	// Create a Tiles Pool
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	void InitializeTilesPool();

	// Create Islands Level Data
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	void GenerateIslands(TArray<class ASIsland*> Islands, bool IsMaritime);

	// Spawn Tiles inside a Island Level
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	void SpawnLevelTiles();
	
	/////////////////////////////////////////////////////////////////////////////////
	
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	FTransform GetBoatSpawnPosition(TArray<ASIsland*> Islands);

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	bool HasIslandLevels() const { return Islands.Num() > 0; }

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	bool IsTilesPoolNotEmpty() const { return TilesPool.Find(TileType::NPT_LANDSCAPE)->Num() > 0; }

	const TArray<FIslandLevel>& GetIslandLevels() const { return Islands; }

	FTile GetRandomRoom(RoomType roomType, BiomeType biome);
	FTile GetRandomTile(TileType TileType, BiomeType biome);

	static BiomeType GetRandomBiomeType() {
		return  static_cast<BiomeType>(FMath::RandRange(0, (int)BiomeType::MAX - 1));
	}


public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FIslandLevel> Islands;
	
	UPROPERTY(BlueprintReadWrite)
	uint8 CurrentIslandID = 255;

private:
	TileType FindTileTypeFromLevelName(const FString& LevelName) const;

	void InitializeIslandLevel(FIslandLevel& level);
	
	void InitializeGrid(TArray<TArray<FTile>>& Grid, TArray<RoomType> RoomPath, BiomeType Biome);

	int GetGridWidth() const { return (ROOM_COUNT * 2) + 3; };
	int GetGridTiles() const { return GetGridWidth() * GetGridWidth(); };

	//UFUNCTION()
	//void OnAllTilesLoaded();

	UFUNCTION()
	void OnTileShown();

	UFUNCTION()
	void UpdateGridVisibility();

	void GetGridCoordFromWorldLocation(FIntPoint& TileCoord, const FVector& WorldLocation);

	float GetWorldLocation(uint8 id);
	FTransform GetTransformFromGridCoordinates(int idx, int idy);

	bool CheckIslandIDValid()
	{
		if (CurrentIslandID == 255 || CurrentIslandID >= Islands.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid IslandID; %d"), CurrentIslandID);
			return false;
		}
		return true;
	}

private:
	bool PoolInitialized = false;

	TMap< TileType, TArray<FTile> > TilesPool;

	TArray<FTile*> TilesToUpdate;

	// Number of rooms in a level island (including start and end rooms)
	const int ROOM_COUNT = 6;

	FIntPoint CurrentPlayerGridCoord;

	static const int SCALE_TILE;
	static const int LANDSCAPE_GRID_SIZE;
	static const int TRANSITION_GRID_SIZE;


	/*
	* Move this in another class
	*/
	int TilesShownNum = 0;
	bool OnLevelBegin = true;

};


// Return Coordinates of the next Room in path depending on the current RoomType and coordinates
inline FVector2D GetNextCoordinate(RoomType RoomType, FVector2D Coordinates) {
	if (RoomType == RoomType::LEFTTOBACK || RoomType == RoomType::BACKTOFRONT || RoomType == RoomType::START_X) return Coordinates + FVector2D(0, 2);
	if (RoomType == RoomType::FRONTTORIGHT || RoomType == RoomType::RIGHTTOLEFT || RoomType == RoomType::START_Y ) return Coordinates + FVector2D(2, 0);
	else return Coordinates;
}

// Return Coordinates of the Playable Transition between current and next Room depending on the current RoomType and coordinates
inline FVector2D GetTransitionCoordinate(RoomType RoomType, FVector2D Coordinates) {
	if (RoomType == RoomType::LEFTTOBACK || RoomType == RoomType::BACKTOFRONT || RoomType == RoomType::START_X) return Coordinates + FVector2D(0, 1);
	if (RoomType == RoomType::FRONTTORIGHT || RoomType == RoomType::RIGHTTOLEFT || RoomType == RoomType::START_Y) return Coordinates + FVector2D(1, 0);
	else return Coordinates;
}


inline bool CheckBiomeAndRoomTypeFromLevelName(const FString& Name, BiomeType Biome, RoomType RoomType)
{
	return Name.Contains(GetBiomeTypeStr(Biome)) && Name.Contains(GetRoomTypeStr(RoomType));
}

inline bool CheckBiomeFromLevelName(const FString& Name, BiomeType Biome)
{
	return Name.Contains(GetBiomeTypeStr(Biome));
}