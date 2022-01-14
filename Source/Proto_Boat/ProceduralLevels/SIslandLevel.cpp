// Fill out your copyright notice in the Description page of Project Settings.


#include "./SIslandLevel.h"
#include "./SRoomTemplate.h"
#include "../GameManager.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

void USIslandLevel::Init(FVector worldPosition, BiomeType biome, bool isMaritime)
{
	WorldPosition = worldPosition;
	Biome = biome;
	IsMaritime = isMaritime;

	// Generate Level (Rooms)
	//FVector nextLocation = FVector(0.0f);

	//USRoomInLevel startRoom;
	//startRoom.PoolIndex = UGameManager::GetRandomRoom(RoomType::START, biome);
	//startRoom.WorldPosition = nextLocation;

	//Rooms.Add(startRoom);

	//auto GM = Cast<UGameManager>(UGameplayStatics::GetGameInstance(GetWorld()));
	//if (GM) {
	//	nextLocation += GM->PoolOfRoom[startRoom.PoolIndex].ExitPosition;
	//}
	//
	//for (int i = 1; i < USIslandLevel::LEVELSIZE - 1; i++) {

	//}
};