// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./ProceduralLevels/SRoomTemplate.h"
#include "./Utility.h"

#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameManager.generated.h"


USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()

public:
	FTile(TileType Type = TileType::PT_LEVELROOM, const FName& Name = "")
		: Type(Type), LevelName(Name) {}

	TileType Type;
	FName LevelName;

};


USTRUCT(BlueprintType)
struct FIslandLevel {
	GENERATED_BODY()

public:
	FIslandLevel() {};
	//  [TO DO] : Lorsqu'on aura assez de room changer la valeur du biome entrée en dur...
	FIslandLevel(FVector worldPosition, BiomeType biome, bool isMaritime, bool isFinished = false)
		:WorldPosition(worldPosition), Biome(BiomeType::FOREST), IsFinished(isFinished), IsMaritime(isMaritime) {}
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

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	void InitializeTilesPool();

	const TArray<FIslandLevel>& GetIslandLevels() const { return Islands; }
	
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	bool HasIslandLevels() const { return Islands.Num() > 0; }

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	bool IsTilesPoolNotEmpty() const { return TilesPool.Find(TileType::NPT_LANDSCAPE)->Num() > 0; }

	UFUNCTION(BlueprintCallable, Category = "GameManager")
	void GenerateIslands(TArray<class ASIsland*> Islands, bool IsMaritime);

	FName GetRandomRoom(RoomType roomType, BiomeType biome);
	FName GetRandomTile(TileType TileType, BiomeType biome);

	static BiomeType GetRandomBiomeType() {
		return  static_cast<BiomeType>(FMath::RandRange(0, (int)BiomeType::MAX - 1));
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FIslandLevel> Islands;


private:
	TileType FindTileTypeFromLevelName(const FName& LevelName) const;

	void InitializeIslandLevel(FIslandLevel& level);
	
	void InitializeGrid(TArray<TArray<FTile>>& Grid, TArray<RoomType> RoomPath, BiomeType Biome);

	int GetGridWidth() const { return (ROOM_COUNT * 2) + 1; };

private:
	bool PoolInitialized = false;

	TMap< TileType, TArray<FTile> > TilesPool;

	// Number of rooms in a level island (including start and end rooms)
	const int ROOM_COUNT = 6;
	
};


// Return Coordinates of the next Room in path depending on the current RoomType and coordinates
inline FVector2D GetNextCoordinate(RoomType RoomType, FVector2D Coordinates) {
	if (RoomType == RoomType::LEFTTOBACK || RoomType == RoomType::BACKTOFRONT) return Coordinates + FVector2D(0, 2);
	if (RoomType == RoomType::FRONTTORIGHT || RoomType == RoomType::RIGHTTOLEFT || RoomType == RoomType::START ) return Coordinates + FVector2D(2, 0);
	else return Coordinates;
}

// Return Coordinates of the Playable Transition between current and next Room depending on the current RoomType and coordinates
inline FVector2D GetTransitionCoordinate(RoomType RoomType, FVector2D Coordinates) {
	if (RoomType == RoomType::LEFTTOBACK || RoomType == RoomType::BACKTOFRONT) return Coordinates + FVector2D(0, 1);
	if (RoomType == RoomType::FRONTTORIGHT || RoomType == RoomType::RIGHTTOLEFT || RoomType == RoomType::START) return Coordinates + FVector2D(1, 0);
	else return Coordinates;
}


inline bool CheckBiomeAndRoomTypeFromLevelName(const FName& Name, BiomeType Biome, RoomType RoomType)
{
	FString NameStr = Name.ToString();
	return NameStr.Contains(GetBiomeTypeStr(Biome)) && NameStr.Contains(GetRoomTypeStr(RoomType));
}

inline bool CheckBiomeFromLevelName(const FName& Name, BiomeType Biome)
{
	FString NameStr = Name.ToString();
	return NameStr.Contains(GetBiomeTypeStr(Biome));
}