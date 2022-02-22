// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlueprintFunctionLibrary.h"

ULevelStreaming* USBlueprintFunctionLibrary::GetStreamingLevelFromActor(AActor* Actor)
{
	if (Actor != nullptr)
	{
		auto Level = Actor->GetLevel();
		FString StreamingLevelName = Level->GetOuter()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *StreamingLevelName);

		UWorld* World = Actor->GetWorld();
		auto STs = World->GetStreamingLevels();
		for (auto ST : STs)
		{
			FString STName = ST->GetWorldAssetPackageName();
			auto LoadedLevel = ST->GetLoadedLevel();
			if (LoadedLevel)
			{
				auto Outer = LoadedLevel->GetOuter();
				if (Outer)
				{
					FString STName2 = LoadedLevel->GetName();
					FString STName3 = Outer->GetName();
					UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *STName);
					UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *STName2);
					UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *STName3);
				}
			}
		}

		auto levels = World->GetLevels();
		for (auto l : levels)
		{
			FString STName = l->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *STName);
		}


		auto StreamingLevel = World->GetLevelStreamingForPackageName(FName(*StreamingLevelName));
		return StreamingLevel;
	}

	return nullptr;
}

AActor* USBlueprintFunctionLibrary::MoveActorToLevelStreaming(AActor* ActorToMove, ULevelStreaming* StreamingLevel)
{
	ULevel* Level = StreamingLevel->GetLoadedLevel();
	if (!Level)
	{
		UE_LOG(LogTemp, Error, TEXT("[MoveActorToLevelStreaming] NULL Level"));
		return nullptr;
	}

	if (!ActorToMove->IsValidLowLevel() || !Level)
	{
		UE_LOG(LogTemp, Error, TEXT("[MoveActorToLevelStreaming] NULL Actor or Level"));
		return nullptr;
	}

	// Remove from world
	UWorld* World = ActorToMove->GetWorld();
	World->RemoveActor(ActorToMove, true);

	// Rename and move it to the level and add it to the list
	ActorToMove->Rename(*ActorToMove->GetName(), Level);
	Level->Actors.Add(ActorToMove);

	return ActorToMove;
}
