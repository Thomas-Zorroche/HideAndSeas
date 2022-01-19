// Fill out your copyright notice in the Description page of Project Settings.


#include "./GameManager.h"
#include "./SProceduralRoom.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

UGameManager::UGameManager() {
	//StreamingLevels = {};
	//PoolOfRoom = {};
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
		
		//UE_LOG(LogTemp, Warning, TEXT("****** LEVEL NAME : %s"), LevelName);

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

// TODO: TO BE REMOVED
//bool UGameManager::AreLevelsShowned() {
//	if (PoolInitialized)
//		return false;
//	for (ULevelStreaming* streamingLevel : StreamingLevels) {
//		if (!streamingLevel->IsLevelVisible())
//			return false;
//	}
//	UE_LOG(LogTemp, Warning, TEXT("levels shown"));
//	return true;
//}


// TODO: TO BE REMOVED
//void UGameManager::InitializeRoomsPool() {
//	for (ULevelStreaming* streamingLevel : StreamingLevels) {
//		ASProceduralRoom* room = Cast<ASProceduralRoom>(streamingLevel->GetLevelScriptActor());
//		if (room) {
//			FSRoomTemplate roomTemplate(streamingLevel->GetWorldAssetPackageFName(), room->GetExitTransform().GetLocation());
//			PoolOfRoom.Add(roomTemplate);
//		}
//		UE_LOG(LogTemp, Warning, TEXT("room template"));
//	}
//	PoolInitialized = true;
//}

void UGameManager::GenerateIslands(TArray<FVector> IslandLocations, bool IsMaritime) {

	for (auto location : IslandLocations) {
		auto level = FIslandLevel(location, GetRandomBiomeType(), IsMaritime) ;
		InitializeIslandLevel(level);
		Islands.Add(level);
	}

}

bool CheckBiomeAndRoomTypeFromLevelName(const FName& Name, BiomeType Biome, RoomType RoomType)
{
	FString NameStr = Name.ToString();
	return NameStr.Contains(GetBiomeTypeStr(Biome)) && NameStr.Contains(GetRoomTypeStr(RoomType));
}

// Find Random room from pool room that is roomtype and biometype, return the index in the PoolOfRooms
int UGameManager::GetRandomRoom(RoomType RoomType, BiomeType Biome) {
	//TArray<int> FilteredPool = FilterByBiomeAndType(biome, roomType);
	//if (FilteredPool.Num() == 0) {
	//	UE_LOG(LogTemp, Warning, TEXT("Filtered Pool is Empty : there is no room with this type and this biome"));
	//	return -1;
	//}

	//return FilteredPool[FMath::RandRange(0, FilteredPool.Num()-1)];

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
	return RoomTiles->IndexOfByKey(RandomTile);
}

TArray<int> UGameManager::FilterByBiomeAndType(BiomeType biome, RoomType roomType) const{
	TArray<int> indices = {};
	for (int i = 0; i < PoolOfRoom.Num(); i++) {
		auto biomtype = PoolOfRoom[i].GetBiomeType();
		auto roomtype = PoolOfRoom[i].GetRoomType();
		if (biomtype == biome && roomtype == roomType)
			indices.Add(i);
	}

	return indices;
}

void UGameManager::InitializeIslandLevel(FIslandLevel& level) {

	// Create Start room
	int StartLevelID = GetRandomRoom(RoomType::START, level.Biome);
    level.Rooms.Add(FRoomInLevel(StartLevelID));

	RoomType PreviousRoomType = RoomType::START;

	// In between Rooms
	for (int i = 1; i < ROOM_COUNT - 1; i++) {
		RoomType NextRoomType = GetRandomRoomType(PreviousRoomType);
		level.Rooms.Add(FRoomInLevel(GetRandomRoom(PreviousRoomType, level.Biome)));
		PreviousRoomType = NextRoomType;
	}

	// Create n-1 room
	RoomType BeforeEndRoomType = IsExitOnYAxis(PreviousRoomType) ? RoomType::LEFTTOBACK : RoomType::BACKTOFRONT;
	level.Rooms.Add(FRoomInLevel(GetRandomRoom(PreviousRoomType, level.Biome)));

	// Create End Room
	level.Rooms.Add(FRoomInLevel(GetRandomRoom(RoomType::END, level.Biome)));

	// Intialize Grid
	level.Grid.Init(FTile(), GetGridWidth() / GetGridWidth());

	// Fill the start room
	level.Grid.SetStartTileID(level.Rooms[0]);
	
	// Fill Grid with Level Room
	for (int i = 1; i < ROOM_COUNT; i++)
	{
		level.Grid.SetNextTileID(level.Rooms[i]);

	}

	// Fill grid tiles with Landscape
	for (int i = 0; i < GetGridWidth(); i++)
	{
		int X = i % (GetGridWidth() + 1);
		int Y = i / GetGridWidth();

		// X even : Room Level column
		if (X % 2 == 0)
		{

		}
		// X odd : Transition column
		else
		{

		}

		// Y even : Room Level row
		if (Y % 2 == 0)
		{

		}
		// Y odd : Transition row
		else
		{

		}
	}
}
