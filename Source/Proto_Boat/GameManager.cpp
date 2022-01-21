// Fill out your copyright notice in the Description page of Project Settings.


#include "./GameManager.h"
#include "./SProceduralRoom.h"
#include "Engine/LevelStreaming.h" 
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SIsland.h"

UGameManager::UGameManager() {
	Islands = {};
	TilesPool = {};

	TilesPool.Add(TileType::NPT_LANDSCAPE, {});
	TilesPool.Add(TileType::NPT_SQUARE, {});
	TilesPool.Add(TileType::NPT_TRANSITION, {});
	TilesPool.Add(TileType::PT_LEVELROOM, {});
	TilesPool.Add(TileType::PT_TRANSITION, {});
}

// Retrieve all streaming levels from "LoadTiles" Map
void UGameManager::InitializeTilesPool() {
	const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();

	uint8 StreamingLevelID = 0;
	for (ULevelStreaming* StreamingLevel : StreamedLevels) {
		FString LevelName = StreamingLevel->GetWorldAssetPackageName();
		
		// Add a tile in the pool
		TileType Type = FindTileTypeFromLevelName(LevelName);
		auto Tiles = TilesPool.Find(Type);
		Tiles->Add(FTile(Type, LevelName, StreamingLevelID));
		
		StreamingLevelID++;
	}

}

TileType UGameManager::FindTileTypeFromLevelName(const FString& LevelName) const
{
	if (LevelName.Contains("NPT_Transition"))	return TileType::NPT_TRANSITION;
	if (LevelName.Contains("PT_Transition"))	return TileType::PT_TRANSITION;
	if (LevelName.Contains("Landscape"))		return TileType::NPT_LANDSCAPE;
	if (LevelName.Contains("Square"))			return TileType::NPT_SQUARE;
	else										return TileType::PT_LEVELROOM;
}

void UGameManager::GenerateIslands(TArray<ASIsland*> IslandActors, bool IsMaritime) {
	for (auto Island : IslandActors) {
		auto level = FIslandLevel(Island->GetActorLocation(), GetRandomBiomeType(), IsMaritime);
		InitializeIslandLevel(level);
		Islands.Add(level);

		Island->SetID(Islands.Num() - 1);
	}
}


FTile UGameManager::GetRandomTile(TileType TileType, BiomeType Biome) {
	// Get tiles
	auto RoomTiles = TilesPool.Find(TileType);

	// Get only tiles with the right biome
	auto FilteredRoomTiles = RoomTiles->FilterByPredicate([Biome](const FTile Tile) {
		return CheckBiomeFromLevelName(Tile.LevelName, Biome);
	});

	// Pick a random tile from the FilteredRoomTiles
	int RandomIndex = FMath::RandRange(0, FilteredRoomTiles.Num() - 1);
	FTile RandomTile = FilteredRoomTiles[RandomIndex];

	// Return ID of TilesPool[TileType::LEVELROOM] 
	//return RoomTiles->IndexOfByKey(RandomTile);
	return FTile(TileType, RandomTile.LevelName, RandomTile.StreamingLevelID);
}

// Find Random room from pool room that is roomtype and biometype, return the index in the PoolOfRooms
FTile UGameManager::GetRandomRoom(RoomType RoomType, BiomeType Biome) {
	// Get level room tiles
	auto RoomTiles = TilesPool.Find(TileType::PT_LEVELROOM);

	// Get only level room with the right biome and type
	auto FilteredRoomTiles = RoomTiles->FilterByPredicate([Biome, RoomType](const FTile Tile) {
		return CheckBiomeAndRoomTypeFromLevelName(Tile.LevelName, Biome, RoomType);
	});

	// Pick a random room from the FilteredRoomTiles
	int RandomIndex = FMath::RandRange(0, FilteredRoomTiles.Num() - 1);
	FTile RandomTile = FilteredRoomTiles[RandomIndex];

	// Return ID of TilesPool[TileType::LEVELROOM] 
	//return RoomTiles->IndexOfByKey(RandomTile);
	return FTile(TileType::PT_LEVELROOM, RandomTile.LevelName, RandomTile.StreamingLevelID);;
}


void UGameManager::InitializeIslandLevel(FIslandLevel& level) {
	// Generate Path of RoomType
	TArray<RoomType> RoomPath = {};

	// Start
	RoomType PreviousRoomType = FMath::RandRange(0, 1) == 0 ? RoomType::START_Y : RoomType::START_X;
	RoomPath.Add(PreviousRoomType);
	level.FinishedStates.Add(true);

	// In between Rooms
	for (int i = 1; i < ROOM_COUNT - 1; i++) {
		level.FinishedStates.Add(false);
		RoomType NextRoomType = GetRandomRoomType(PreviousRoomType);
		RoomPath.Add(NextRoomType);
		PreviousRoomType = NextRoomType;
	}

	// Create End Room
	level.FinishedStates.Add(false);
	RoomPath.Add(GetRandomEndType(PreviousRoomType));

	// Initialize Grid
	InitializeGrid(level.Grid, RoomPath, level.Biome);
}

void UGameManager::InitializeGrid(TArray<TArray<FTile>>& Grid, TArray<RoomType> RoomPath, BiomeType Biome) {
	// Initialize Grid Content with all NPT
	Grid.Init(TArray<FTile>(), GetGridWidth());
	for (int x = 0; x < GetGridWidth(); x++) {
		Grid[x].Init(FTile(), GetGridWidth());
		for (int y = 0; y < GetGridWidth(); y++) {
			//FVector2D CurrentCoord(x, y);
			if (x % 2 != 0 && y % 2 != 0)
				Grid[x][y] = GetRandomTile(TileType::NPT_SQUARE, Biome);
			else if (x % 2 != 0 || y % 2 != 0)
				Grid[x][y] = GetRandomTile(TileType::NPT_TRANSITION, Biome);
			else if (x % 2 == 0 && y % 2 == 0)
				Grid[x][y] = GetRandomTile(TileType::NPT_LANDSCAPE, Biome);
		}
	}

	// Update Tiles along Path coord to be Playable Tiles (Rooms and Transitions)
	FVector2D PreviousCoord = FVector2D(2, 2);
	Grid[PreviousCoord.X][PreviousCoord.Y] = GetRandomRoom(RoomPath[0], Biome);
	FVector2D StartTransitionCoord = GetTransitionCoordinate(RoomPath[0], PreviousCoord);
	Grid[StartTransitionCoord.X][StartTransitionCoord.Y] = GetRandomTile(TileType::PT_TRANSITION, Biome);

	for (int i = 0; i < RoomPath.Num() - 2; i++) {
		RoomType PreviousRoomType = RoomPath[i];
		RoomType NextRoomType = RoomPath[i+1];
		FVector2D NextCoord = GetNextCoordinate(PreviousRoomType, PreviousCoord);
		FVector2D PTTransitionCoord = GetTransitionCoordinate(NextRoomType, NextCoord);
		auto x = NextCoord.X;
		auto y = NextCoord.Y;
		Grid[NextCoord.X][NextCoord.Y] = GetRandomRoom(NextRoomType, Biome);
		Grid[PTTransitionCoord.X][PTTransitionCoord.Y] = GetRandomTile(TileType::PT_TRANSITION, Biome);
		PreviousCoord = NextCoord;
	}

	FVector2D NextCoord = GetNextCoordinate(RoomPath.Last(1) /* avant dernier */, PreviousCoord);
	int x = NextCoord.X;
	int y = NextCoord.Y;
	Grid[x][y] = GetRandomRoom(RoomPath.Last(0), Biome);
}

float GetWorldLocation(uint8 id, int TRANSITION_GRID_SIZE, int LANDSCAPE_GRID_SIZE)
{

	if (id % 2 == 0)
		return (id / 2) * (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE) + 0.5 * LANDSCAPE_GRID_SIZE;
	else
		return ((id / 2) + 1) * LANDSCAPE_GRID_SIZE + (id / 2) * TRANSITION_GRID_SIZE + 0.5 * TRANSITION_GRID_SIZE;
}

FTransform GetTransformFromGridCoordinates(int idx, int idy)
{
	static int SCALE = 100;
	static int LANDSCAPE_GRID_SIZE = 63 * SCALE;

	FTransform Transform;

	// Location (Swap axes because of unreal axis)
	float WorldLocationX = GetWorldLocation(idy, 10 * SCALE, LANDSCAPE_GRID_SIZE);
	float WorldLocationY = GetWorldLocation(idx, 10 * SCALE, LANDSCAPE_GRID_SIZE);

	// Rotation
	if (idx % 2 != 0 && idy % 2 == 0)
	{
		FRotator Rotator = { 0, 90, 0 };
		Transform.SetRotation(Rotator.Quaternion());
		WorldLocationX += 0.5 * SCALE;
		WorldLocationY += 0.5 * SCALE;
	}

	Transform.SetLocation(FVector(WorldLocationX, WorldLocationY, 0.0f));
	return Transform;
}

void UGameManager::SpawnLevelTiles()
{
	if (CurrentIslandID == 255 || CurrentIslandID >= Islands.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid IslandID; %d"), CurrentIslandID);
		return;
	}

	FIslandLevel& CurrentIslandLevel = Islands[CurrentIslandID];
	const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();
	for (size_t idx = 0; idx < CurrentIslandLevel.Grid.Num(); idx++)
	{
		for (size_t idy = 0; idy < CurrentIslandLevel.Grid.Num(); idy++)
		{
			FTile& Tile = CurrentIslandLevel.Grid[idx][idy];
			ULevelStreaming* StreamingLevel = StreamedLevels[Tile.StreamingLevelID];
			FString UniqueName = "Tile" + FString::FromInt(idx) + "_" + FString::FromInt(idy);

			auto LevelInstance = StreamingLevel->CreateInstance(UniqueName);
			LevelInstance->LevelTransform = FTransform(GetTransformFromGridCoordinates(idx, idy));

			LevelInstance->SetShouldBeVisible(true);
			LevelInstance->SetShouldBeLoaded(true);
		}
	}
}

