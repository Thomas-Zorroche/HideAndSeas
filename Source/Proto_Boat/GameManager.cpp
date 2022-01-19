// Fill out your copyright notice in the Description page of Project Settings.


#include "./GameManager.h"
#include "./SProceduralRoom.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

UGameManager::UGameManager() {
	StreamingLevels = {};
	PoolOfRoom = {};
	Islands = {};
}

void UGameManager::GetStreamingLevels() {
	const TArray<ULevelStreaming*>& streamedLevels = GetWorld()->GetStreamingLevels();
	for (ULevelStreaming* streamingLevel : streamedLevels) {
		FString levelName = streamingLevel->GetWorldAssetPackageName();
		UE_LOG(LogTemp, Warning, TEXT("****** LEVEL NAME : %s"), *levelName);

		streamingLevel->SetShouldBeLoaded(true);
		streamingLevel->SetShouldBeVisible(true);
		StreamingLevels.Add(streamingLevel);
	}
}

bool UGameManager::AreLevelsShowned() {
	if (PoolInitialized)
		return false;
	for (ULevelStreaming* streamingLevel : StreamingLevels) {
		if (!streamingLevel->IsLevelVisible())
			return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("levels shown"));
	return true;
}

void UGameManager::InitializeRoomsPool() {
	for (ULevelStreaming* streamingLevel : StreamingLevels) {
		ASProceduralRoom* room = Cast<ASProceduralRoom>(streamingLevel->GetLevelScriptActor());
		if (room) {
			FSRoomTemplate roomTemplate(streamingLevel->GetWorldAssetPackageFName(), room->GetExitTransform().GetLocation());
			PoolOfRoom.Add(roomTemplate);
		}
		UE_LOG(LogTemp, Warning, TEXT("room template"));
	}
	PoolInitialized = true;
}

void UGameManager::GenerateIslands(TArray<FVector> IslandLocations, bool IsMaritime) {

	for (auto location : IslandLocations) {
		auto level = FIslandLevel(location, GetRandomBiomeType(), IsMaritime) ;
		InitializeIslandLevel(level);
		Islands.Add(level);
	}

}


// Find Random room from pool room that is roomtype and biometype, return the index in the PoolOfRooms
int UGameManager::GetRandomRoom(RoomType roomType, BiomeType biome) {
	TArray<int> FilteredPool = FilterByBiomeAndType(biome, roomType);
	if (FilteredPool.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Filtered Pool is Empty : there is no room with this type and this biome"));
		return -1;
	}

	return FilteredPool[FMath::RandRange(0, FilteredPool.Num()-1)];
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
	// Generate Level (Rooms)
	FVector nextLocation = FVector(0.0f);
	int i = 0;

	// Create Start room
	int id = GetRandomRoom(RoomType::START, level.Biome);
 	level.Rooms.Add(FRoomInLevel(id, nextLocation));

	nextLocation += PoolOfRoom[level.Rooms[i].PoolIndex].ExitPosition;
	RoomType previousRoomType = RoomType::START;
	i++;

	// In between Rooms
	for (; i < 6 - 1; i++) {
		previousRoomType = GetRandomRoomType(previousRoomType);
		level.Rooms.Add(FRoomInLevel(GetRandomRoom(previousRoomType, level.Biome), nextLocation));
		nextLocation += PoolOfRoom[level.Rooms[i].PoolIndex].ExitPosition;
	}

	// Create n-1 room
	previousRoomType = IsExitOnYAxis(previousRoomType) ? RoomType::LEFTTOBACK : RoomType::BACKTOFRONT;
	level.Rooms.Add(FRoomInLevel(GetRandomRoom(previousRoomType, level.Biome), nextLocation));
	nextLocation += PoolOfRoom[level.Rooms[i].PoolIndex].ExitPosition;

	i++;

	// Create End Room
	level.Rooms.Add(FRoomInLevel(GetRandomRoom(RoomType::END, level.Biome), nextLocation));

}
