// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Utility.h"
#include "../SPatrolPath.h"
#include "../SLevelLight.h"
#include "Math/IntPoint.h" 

#include "SLevelManager.generated.h"

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()

public:
	FTile(TileType Type = TileType::PT_LEVELROOM, bool isCompleted = false, FString Name = "", uint8 slID = 0)
		: Type(Type), IsCompleted(isCompleted), LevelName(Name), StreamingLevelID(slID) {}

	TileType Type;
	FString LevelName;

	// Common ID in the GetWorld().StreamingLevels
	uint8 StreamingLevelID = 0;

	// Unique ID in the grid
	uint8 GridID = 0;

	// If the room is completed (Only for LEVELROOM)
	bool IsCompleted;

	// True first time the level streaming is shown. False after. 
	bool FirstTimeShown = true;

	TArray<ASPatrolPath*> PatrollerPaths;

	TArray<class ASCamera*> Cameras;

	TArray<ASLevelLight*> LevelLights;

	void FillActors(TArray<AActor*> PatrollerPathActors, TArray<AActor*> Cameras,  TArray<AActor*> LevelLightActors, const TArray<ULevelStreaming*>& StreamingLevels);

	void OnTileShown();

	void SetPlayerTile(bool IsPlayerTile);
};


USTRUCT(BlueprintType)
struct FIslandLevel
{
	GENERATED_BODY()

public:
	FIslandLevel() {};
	//  [TO DO] : Lorsqu'on aura assez de tiles changer la valeur du biome entr�e en dur...
	FIslandLevel(FTransform transform, uint8 id, BiomeType biome, bool isMaritime)
		: Transform(transform), ID(id), Biome(BiomeType::FOREST /* biome */), IsMaritime(isMaritime) {}

	UPROPERTY(BlueprintReadOnly)
	FTransform Transform;

	UPROPERTY(BlueprintReadOnly)
	uint8 ID;
	BiomeType Biome;
	bool IsMaritime;

	TArray<TArray<FTile>> Grid;

	bool GridLoaded = false;
};


UCLASS()
class PROTO_BOAT_API USLevelManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize();

	// Create a Tiles Pool
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void InitializeTilesPool();

	// Create Islands Level Data
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void GenerateIslands(TArray<class ASIsland*> Islands, bool IsMaritime);

	// Load Streaming Levels inside an Island Level
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void LoadLevelTiles();

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	bool HasIslandLevels() const { return Islands.Num() > 0; }

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	bool IsTilesPoolNotEmpty() const { return TilesPool.Find(TileType::NPT_LANDSCAPE)->Num() > 0; }

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void FinishCurrentIsland();

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	bool isIslandFinished(const uint8 islandId) { return FinishedIslands.Contains(islandId); }
	
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	FColor GetCrystalColorOfCurrentIsland();

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void CompleteRoom(FVector worldLocation);

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void ClearGridTimer();

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	TArray<class ASPatrolPath*> GetPatrollersFromActorTile(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	TArray<AActor*> GetAllEnemiesFromPlayerTile();

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void AddDiscoveredIsland(uint8 islandID) { if(!DiscoveredIslands.Contains(islandID)) DiscoveredIslands.Add(islandID); }

	const TArray<FIslandLevel>& GetIslandLevels() const { return Islands; }

	FTile GetRandomRoom(RoomType roomType, BiomeType biome);
	FTile GetRandomTile(TileType TileType, BiomeType biome);

	BiomeType GetRandomBiomeType() { return  static_cast<BiomeType>(FMath::RandRange(0, (int)BiomeType::MAX - 1)); }

	bool CheckIslandIDValid() const;

	uint8 GetCurrentIslandID() const { return CurrentIslandID; }

public:
	
	// Array of Levels
	UPROPERTY(BlueprintReadOnly)
	TArray<FIslandLevel> Islands;

	// Last visited island
	UPROPERTY(BlueprintReadWrite)
	uint8 CurrentIslandID = 255;

	// Array which contains IDs of finished islands
	UPROPERTY(BlueprintReadOnly)
	TArray<uint8> FinishedIslands;

	// Array which contains IDs of discovered islands
	UPROPERTY(BlueprintReadOnly)
	TArray<uint8> DiscoveredIslands;

	UPROPERTY(BlueprintReadOnly)
	TArray<FColor> CrystalColors;

	FTimerHandle GridTimerHandle;

private:
	void InitializeCrystalColors();

	TileType FindTileTypeFromLevelName(const FString& LevelName) const;

	void InitializeIslandLevel(FIslandLevel& level);

	void InitializeGrid(TArray<TArray<FTile>>& Grid, TArray<RoomType> RoomPath, BiomeType Biome);

	int GetGridWidth() const { return (LEVELROOMS_COUNT * 2) + 3; };
	int GetGridTiles() const { return GetGridWidth() * GetGridWidth(); };

	UFUNCTION()
	void OnTileShown();

	UFUNCTION()
	void UpdateGridVisibility();

	void GetGridCoordFromWorldLocation(FIntPoint& TileCoord, const FVector& WorldLocation);

	float GetTileLocationFromGridCoordinate(int index);

	FTransform GetTileTransformFromGridCoordinates(int idx, int idy);

private:
	bool PoolInitialized = false;

	TMap< TileType, TArray<FTile> > TilesPool;

	TArray<FTile*> TilesToUpdate;

	const int LEVELROOMS_COUNT = 6;

	FIntPoint CurrentPlayerGridCoord;

	const int SCALE_TILE = 100;
	const int LANDSCAPE_GRID_SIZE = 63 * SCALE_TILE;
	const int TRANSITION_GRID_SIZE = 10 * SCALE_TILE;

	int TilesShownNum = 0;

	bool OnLevelBegin = true;
	
};
