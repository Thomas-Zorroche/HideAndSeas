// Fill out your copyright notice in the Description page of Project Settings.


#include "./GameManager.h"
#include "./SProceduralRoom.h"
#include "Engine/LevelStreaming.h" 
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SIsland.h"
#include "Interfaces/SLevelLoaded.h"
#include "GameFramework/Character.h"
#include "Engine/LevelBounds.h" 


const int UGameManager::SCALE_TILE		     = 100;
const int UGameManager::LANDSCAPE_GRID_SIZE  = 63 * SCALE_TILE;
const int UGameManager::TRANSITION_GRID_SIZE = 10 * SCALE_TILE;


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
	if (LevelName.Contains("Water"))			return TileType::NPT_SQUARE;
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

float UGameManager::GetWorldLocation(uint8 id)
{
	if (id % 2 == 0)
		return (id / 2) * (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE) + 0.5 * LANDSCAPE_GRID_SIZE;
	else
		return ((id / 2) + 1) * LANDSCAPE_GRID_SIZE + (id / 2) * TRANSITION_GRID_SIZE + 0.5 * TRANSITION_GRID_SIZE;
}

FTransform UGameManager::GetTransformFromGridCoordinates(int idx, int idy)
{
	FTransform Transform;

	// Location (Swap axes because of unreal axis)
	float WorldLocationX = GetWorldLocation(idy);
	float WorldLocationY = GetWorldLocation(idx);

	// Rotation
	if (idx % 2 != 0 && idy % 2 == 0)
	{
		FRotator Rotator = { 0, 90, 0 };
		Transform.SetRotation(Rotator.Quaternion());
		WorldLocationX += 0.5 * SCALE_TILE;
		WorldLocationY += 0.5 * SCALE_TILE;
	}

	Transform.SetLocation(FVector(WorldLocationX, WorldLocationY, 0.0f));
	return Transform;
}

bool ShouldTileBeVisible(FIntPoint Tile, FIntPoint PlayerTile, int NeighboursCount = 2)
{
	return FMath::Abs((int)Tile.X - PlayerTile.Y) <= NeighboursCount
		&& FMath::Abs((int)Tile.Y - PlayerTile.X) <= NeighboursCount;
}

// TODO Rename to "LoadLevelTiles"
void UGameManager::SpawnLevelTiles()
{
	if (!CheckIslandIDValid())
		return;

	CurrentPlayerGridCoord = { 2, 2 };
	FIslandLevel& CurrentIslandLevel = Islands[CurrentIslandID];
	const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();
	uint8 StreamingLevelID = StreamedLevels.Num();
	for (size_t idx = 0; idx < CurrentIslandLevel.Grid.Num(); idx++)
	{
		for (size_t idy = 0; idy < CurrentIslandLevel.Grid.Num(); idy++)
		{
			// Special case for water ( (0,2) & (1,2) )
			if (idy == 2 && (idx == 0 || idx == 1))
			{
				continue;
			}

			FTile& Tile = CurrentIslandLevel.Grid[idx][idy];
			ULevelStreaming* StreamingLevel = StreamedLevels[Tile.StreamingLevelID];
			FString UniqueName = "Tile" + FString::FromInt(idx) + "_" + FString::FromInt(idy);

			auto LevelInstance = StreamingLevel->CreateInstance(UniqueName);
			LevelInstance->LevelTransform = FTransform(GetTransformFromGridCoordinates(idx, idy));

			Tile.StreamingLevelID = StreamingLevelID;
			// Update StreamingLevelID
			StreamingLevelID++;

			LevelInstance->SetShouldBeLoaded(true);
			if (ShouldTileBeVisible(FIntPoint(idx, idy), CurrentPlayerGridCoord))
			{
				TilesToUpdate.Add(&Tile);	
				LevelInstance->SetShouldBeVisible(true);
				LevelInstance->OnLevelShown.AddDynamic(this, &UGameManager::OnTileShown);
			}
		}
	}
}



//void UGameManager::OnAllTilesLoaded()
//{
//	TilesLoadedInLevel++;
//	if (TilesLoadedInLevel != 25)
//	{
//		return;
//	}
//
//	UE_LOG(LogTemp, Warning, TEXT("ALL TILES LOADED"));
//
//	// Event for grid update
//	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
//	FTimerDelegate TimerDel;
//	FTimerHandle TimerHandle;
//	TimerDel.BindUFunction(this, FName("UpdateGridVisibility"));
//	GetTimerManager().SetTimer(TimerHandle, TimerDel, 5.f, true /* loop */);
//
//	// Warn all actors that implements LevelLoaded interface
//	TArray<AActor*> Actors;
//	UGameplayStatics::GetAllActorsWithInterface(GWorld, USLevelLoaded::StaticClass(), Actors);
//	for (auto Actor : Actors)
//	{
//		ISLevelLoaded* LevelLoadedInterface = Cast<ISLevelLoaded>(Actor);
//		if (LevelLoadedInterface)
//		{
//			LevelLoadedInterface->Execute_OnLevelLoaded(Actor);
//		}
//	}
//}

void UGameManager::OnTileShown()
{
	TilesShownNum++;
	// Wait until all tiles are shown (tiles of any type in the TilesToUpdate list)
	int MaxTiles = OnLevelBegin ? 23 : 10;
	if (TilesShownNum != MaxTiles)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("TILES UPDATE"));

	// Retrieve only LevelRooms
	const auto LevelRoomTiles = TilesToUpdate.FilterByPredicate([](FTile* Tile)
		{
			return Tile->Type == TileType::PT_LEVELROOM;
		});


	// Search if one tile is FirstTimeShow
	bool NeedToSearchPatrollers = false;
	for (FTile* Tile : LevelRoomTiles)
	{
		if (Tile->FirstTimeShown)
		{
			NeedToSearchPatrollers = true;
			break;
		}
	}

	// If there is at least one, find all patrol paths on the map
	TArray<AActor*> PatrolPaths;
	if (NeedToSearchPatrollers)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASPatrolPath::StaticClass(), PatrolPaths);
	}

	// Update Tiles
	for (FTile* Tile : LevelRoomTiles)
	{
		if (Tile->FirstTimeShown)
			Tile->FillPatrollerPaths(PatrolPaths, GetWorld()->GetStreamingLevels());
		Tile->OnTileShown();
	}

	// Event for grid update
	if (OnLevelBegin)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;
		TimerDel.BindUFunction(this, FName("UpdateGridVisibility"));
		GetTimerManager().SetTimer(TimerHandle, TimerDel, 5.f, true /* loop */);

		// Warn all actors that implements LevelLoaded interface
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsWithInterface(GWorld, USLevelLoaded::StaticClass(), Actors);
		for (auto Actor : Actors)
		{
			ISLevelLoaded* LevelLoadedInterface = Cast<ISLevelLoaded>(Actor);
			if (LevelLoadedInterface)
			{
				LevelLoadedInterface->Execute_OnLevelLoaded(Actor);
			}
		}
	}

	// Reset states
	TilesToUpdate.Empty();
	TilesShownNum = 0;
	OnLevelBegin = false;
}

void UGameManager::GetGridCoordFromWorldLocation(FIntPoint& TileCoord, const FVector& WorldLocation)
{
	TileCoord.X = ((int)WorldLocation.X / (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE)) * 2;
	TileCoord.Y = ((int)WorldLocation.Y / (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE)) * 2;
}

void UGameManager::UpdateGridVisibility()
{
	auto Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
		return; 

	FIntPoint NewPlayerGridCoord;
	GetGridCoordFromWorldLocation(NewPlayerGridCoord, Player->GetActorLocation());

	if (NewPlayerGridCoord == CurrentPlayerGridCoord)
		return;

	CurrentPlayerGridCoord = NewPlayerGridCoord;
	UE_LOG(LogTemp, Warning, TEXT("UPDATE GRID: NEW TILE : %d , %d"), CurrentPlayerGridCoord.X, CurrentPlayerGridCoord.Y);

	if (!CheckIslandIDValid())
		return;

	FIslandLevel& CurrentIslandLevel = Islands[CurrentIslandID];
	const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();
	TilesToUpdate.Empty();
	for (size_t idx = 0; idx < CurrentIslandLevel.Grid.Num(); idx++)
	{
		for (size_t idy = 0; idy < CurrentIslandLevel.Grid.Num(); idy++)
		{
			FTile& Tile = CurrentIslandLevel.Grid[idx][idy];
			ULevelStreaming* StreamingLevel = StreamedLevels[Tile.StreamingLevelID];
			bool ShouldBeVisible = ShouldTileBeVisible(FIntPoint(idx, idy), CurrentPlayerGridCoord, 2);
			
			if (ShouldBeVisible && StreamingLevel->GetShouldBeVisibleFlag())
			{
				continue;
			}
			else if (ShouldBeVisible)
			{
				StreamingLevel->SetShouldBeVisible(true);
				TilesToUpdate.Add(&Tile);
				StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &UGameManager::OnTileShown);
			}
			else
			{
				StreamingLevel->SetShouldBeVisible(false);
			}
		}
	}
}

FTransform UGameManager::GetBoatSpawnPosition(TArray<ASIsland*> ActorIslands) 
{
	for (auto Island : ActorIslands) {
		if (Island->GetID() == CurrentIslandID) {
			return Island->ExitTransform;
		}
	}
	return FTransform();
}


/////////////////////////////////////////////////////////////////////////////////////


void FTile::FillPatrollerPaths(TArray<AActor*> Actors, const TArray<ULevelStreaming*>& StreamingLevels)
{
	if (Type != TileType::PT_LEVELROOM)
		return;

	const auto StreamingLevel = StreamingLevels[StreamingLevelID];
	FBox TileBox = ALevelBounds::CalculateLevelBounds(StreamingLevel->GetLoadedLevel());

	if (!TileBox.IsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FTile::FillPatrollerPaths] FBox Invalid."));
		return;
	}

	// Retrieve all Patroller Paths inside the tile
	for (auto Actor : Actors)
	{
		if (TileBox.IsInsideXY(Actor->GetActorLocation()))
		{
			auto PatrolPath = Cast<ASPatrolPath>(Actor);
			if (PatrolPath)
			{
				PatrollerPaths.Add(PatrolPath);
			}
		}
	}
}

void FTile::OnTileShown()
{
	if (Type != TileType::PT_LEVELROOM)
		return;

	if (PatrollerPaths.Num() == 0)
	{
		FirstTimeShown = false;
		return;
	}

	// Create Patrollers
	UE_LOG(LogTemp, Warning, TEXT("OnTileShown %d, patrol path : %d, FirstTimeShown : %d"), StreamingLevelID, PatrollerPaths.Num(), FirstTimeShown);
	if (FirstTimeShown)
	{
		for (auto PatrollerPath : PatrollerPaths)
		{
			UE_LOG(LogTemp, Warning, TEXT("CREATE PATROLLER"));
			PatrollerPath->CreatePatroller();
		}
		FirstTimeShown = false;
	}
	else
	{
		for (auto PatrollerPath : PatrollerPaths)
		{
			PatrollerPath->ResetPatroller();
			UE_LOG(LogTemp, Warning, TEXT("RESET PATROLLER"));
		}
	}
}