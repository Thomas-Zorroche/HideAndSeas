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
		//ASProceduralRoom* room = Cast<ASProceduralRoom>(streamingLevel->GetLevelScriptActor());

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
