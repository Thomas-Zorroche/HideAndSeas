// Fill out your copyright notice in the Description page of Project Settings.


#include "./GameManager.h"
#include "./SProceduralRoom.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
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

	for (ULevelStreaming* StreamingLevel : StreamedLevels) {
		FName LevelName = StreamingLevel->GetWorldAssetPackageFName();
		// Add a tile in the pool
		TileType Type = FindTileTypeFromLevelName(LevelName);
		auto Tiles = TilesPool.Find(Type);
		Tiles->Add(FTile(Type, LevelName));
	}

}

TileType UGameManager::FindTileTypeFromLevelName(const FName& LevelName) const
{
	FString LevelNameStr = LevelName.ToString();
	if (LevelNameStr.Contains("NPT_Transition"))	return TileType::NPT_TRANSITION;
	if (LevelNameStr.Contains("PT_Transition"))		return TileType::PT_TRANSITION;
	if (LevelNameStr.Contains("Landscape"))			return TileType::NPT_LANDSCAPE;
	if (LevelNameStr.Contains("Square"))			return TileType::NPT_SQUARE;
	else											return TileType::PT_LEVELROOM;
}

void UGameManager::GenerateIslands(TArray<ASIsland*> IslandActors, bool IsMaritime) {
	for (auto Island : IslandActors) {
		auto level = FIslandLevel(Island->GetActorLocation(), GetRandomBiomeType(), IsMaritime);
		InitializeIslandLevel(level);
		Islands.Add(level);

		Island->SetID(Islands.Num() - 1);
	}
}


FName UGameManager::GetRandomTile(TileType TileType, BiomeType Biome) {
	// Get level room tiles
	auto RoomTiles = TilesPool.Find(TileType);

	// Get only level room with the right biome and type
	auto FilteredRoomTiles = RoomTiles->FilterByPredicate([Biome](const FTile Tile) {
		return CheckBiomeFromLevelName(Tile.LevelName, Biome);
	});

	// Pick a random room from the FilteredRoomTiles
	int RandomIndex = FMath::RandRange(0, FilteredRoomTiles.Num() - 1);
	FTile RandomTile = FilteredRoomTiles[RandomIndex];

	// Return ID of TilesPool[TileType::LEVELROOM] 
	//return RoomTiles->IndexOfByKey(RandomTile);
	return RandomTile.LevelName;
}

// Find Random room from pool room that is roomtype and biometype, return the index in the PoolOfRooms
FName UGameManager::GetRandomRoom(RoomType RoomType, BiomeType Biome) {
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
	return RandomTile.LevelName;
}


void UGameManager::InitializeIslandLevel(FIslandLevel& level) {
	// Generate Path of RoomType
	TArray<RoomType> RoomPath = {};

	// Start
	RoomPath.Add(RoomType::START);
	RoomType PreviousRoomType = RoomType::START;
	level.FinishedStates.Add(true);

	// In between Rooms
	for (int i = 1; i < ROOM_COUNT - 2; i++) {
		level.FinishedStates.Add(false);
		RoomType NextRoomType = GetRandomRoomType(PreviousRoomType);
		RoomPath.Add(NextRoomType);
		PreviousRoomType = NextRoomType;
	}

	// Create n-1 room
	level.FinishedStates.Add(false);
	RoomType BeforeEndRoomType = IsExitOnYAxis(PreviousRoomType) ? RoomType::LEFTTOBACK : RoomType::BACKTOFRONT;
	RoomPath.Add(BeforeEndRoomType);

	// Create End Room
	level.FinishedStates.Add(false);
	RoomPath.Add(RoomType::END);

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
				Grid[x][y] = FTile(TileType::NPT_SQUARE, GetRandomTile(TileType::NPT_SQUARE, Biome));
			else if (x % 2 != 0 || y % 2 != 0)
				Grid[x][y] = FTile(TileType::NPT_TRANSITION, GetRandomTile(TileType::NPT_TRANSITION, Biome));
			else if (x % 2 == 0 && y % 2 == 0)
				Grid[x][y] = FTile(TileType::NPT_LANDSCAPE, GetRandomTile(TileType::NPT_LANDSCAPE, Biome));
		}
	}

	// Update Tiles along Path coord to be Playable Tiles (Rooms and Transitions)
	FVector2D PreviousCoord = FVector2D(2, 2);
	Grid[PreviousCoord.X][PreviousCoord.Y] = FTile(TileType::PT_LEVELROOM, GetRandomRoom(RoomType::START, Biome));
	Grid[3][2] = FTile(TileType::PT_TRANSITION, GetRandomTile(TileType::PT_TRANSITION, Biome));

	for (int i = 0; i < RoomPath.Num() - 1; i++) {
		RoomType PreviousRoomType = RoomPath[i];
		RoomType NextRoomType = RoomPath[i+1];
		FVector2D NextCoord = GetNextCoordinate(PreviousRoomType, PreviousCoord);
		FVector2D PTTransitionCoord = GetTransitionCoordinate(NextRoomType, NextCoord);
		Grid[NextCoord.X][NextCoord.Y] = FTile(TileType::PT_LEVELROOM, GetRandomRoom(NextRoomType, Biome));
		Grid[PTTransitionCoord.X][PTTransitionCoord.Y] = FTile(TileType::PT_TRANSITION, GetRandomTile(TileType::PT_TRANSITION, Biome));
		PreviousCoord = NextCoord;
	}

	FVector2D NextCoord = GetNextCoordinate(RoomPath.Last(1) /* avant dernier */, PreviousCoord);
	int x = NextCoord.X;
	int y = NextCoord.Y;
	Grid[x][y] = FTile(TileType::PT_LEVELROOM, GetRandomRoom(RoomType::END, Biome));

}
