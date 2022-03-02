#include "SLevelManager.h"

#include "Engine/LevelStreaming.h" 
#include "Engine/LevelBounds.h" 
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "../SPatroller.h"
#include "../SCamera.h"

#include "../SIsland.h"
#include "../Interfaces/SLevelLoaded.h"


void USLevelManager::Initialize() 
{
	Islands = {};
	TilesPool = {};
	InitializeCrystalColors();

	TilesPool.Add(TileType::NPT_LANDSCAPE, {});
	TilesPool.Add(TileType::NPT_SQUARE, {});
	TilesPool.Add(TileType::NPT_TRANSITION, {});
	TilesPool.Add(TileType::PT_LEVELROOM, {});
	TilesPool.Add(TileType::PT_TRANSITION, {});
}

void USLevelManager::ClearGridTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(GridTimerHandle);
}

void USLevelManager::InitializeCrystalColors()
{
	for (const auto Island : Islands)
	{
		FColor RandomColor = FColor::MakeRandomColor();
		CrystalColors.Add(RandomColor);
	}
}

FColor USLevelManager::GetCrystalColorOfCurrentIsland()
{
	return CrystalColors[CurrentIslandID];
}

bool USLevelManager::CheckIslandIDValid() const
{
	if (CurrentIslandID == 255 || CurrentIslandID >= Islands.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid IslandID: %d"), (int)CurrentIslandID);
		return false;
	}
	return true;
}

void USLevelManager::FinishCurrentIsland() {
	const uint8 id = GetCurrentIslandID();
	// Add the current island ID to the list of finished islands
	if (!FinishedIslands.Contains(id)) {
		FinishedIslands.Add(id);
	}
}

// Retrieve all streaming levels from "LoadTiles" Map
void USLevelManager::InitializeTilesPool() 
{
	const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();

	uint8 StreamingLevelID = 0;
	for (ULevelStreaming* StreamingLevel : StreamedLevels) {
		FString LevelName = StreamingLevel->GetWorldAssetPackageName();

		// Add a tile in the pool
		TileType Type = FindTileTypeFromLevelName(LevelName);
		auto Tiles = TilesPool.Find(Type);
		Tiles->Add(FTile(Type, false, LevelName, StreamingLevelID));

		StreamingLevelID++;
	}

}

TileType USLevelManager::FindTileTypeFromLevelName(const FString& LevelName) const
{
	if (LevelName.Contains("NPT_Transition"))	return TileType::NPT_TRANSITION;
	if (LevelName.Contains("PT_Transition"))	return TileType::PT_TRANSITION;
	if (LevelName.Contains("Landscape"))		return TileType::NPT_LANDSCAPE;
	if (LevelName.Contains("Square"))			return TileType::NPT_SQUARE;
	if (LevelName.Contains("Water"))			return TileType::NPT_SQUARE;
	else										return TileType::PT_LEVELROOM;
}

void USLevelManager::GenerateIslands(TArray<ASIsland*> IslandActors, bool IsMaritime) 
{
	for (auto Island : IslandActors) {
		const uint8 islandId = Islands.Num();
		auto level = FIslandLevel(Island->GetActorTransform(), islandId, GetRandomBiomeType(), IsMaritime);
		InitializeIslandLevel(level);
		Islands.Add(level);

		Island->SetID(islandId);
	}
	InitializeCrystalColors();
}


inline bool CheckBiomeFromLevelName(const FString& Name, BiomeType Biome)
{
	return Name.Contains(GetBiomeTypeStr(Biome));
}

FTile USLevelManager::GetRandomTile(TileType TileType, BiomeType Biome) 
{
	// Get tiles
	auto RoomTiles = TilesPool.Find(TileType);

	// Get only tiles with the right biome
	auto FilteredRoomTiles = RoomTiles->FilterByPredicate([Biome](const FTile Tile) 
		{
			return CheckBiomeFromLevelName(Tile.LevelName, Biome);
		});

	// Pick a random tile from the FilteredRoomTiles
	int RandomIndex = FMath::RandRange(0, FilteredRoomTiles.Num() - 1);
	FTile RandomTile = FilteredRoomTiles[RandomIndex];

	// Return ID of TilesPool[TileType::LEVELROOM] 
	//return RoomTiles->IndexOfByKey(RandomTile);
	return FTile(TileType, false, RandomTile.LevelName, RandomTile.StreamingLevelID);
}

inline bool CheckBiomeAndRoomTypeFromLevelName(const FString& Name, BiomeType Biome, RoomType RoomType)
{
	return Name.Contains(GetBiomeTypeStr(Biome)) && Name.Contains(GetRoomTypeStr(RoomType));
}

// Find Random room from pool room that is roomtype and biometype, return the index in the PoolOfRooms
FTile USLevelManager::GetRandomRoom(RoomType RoomType, BiomeType Biome) 
{
	// Get level room tiles
	auto RoomTiles = TilesPool.Find(TileType::PT_LEVELROOM);

	// Get only level room with the right biome and type
	auto FilteredRoomTiles = RoomTiles->FilterByPredicate([Biome, RoomType](const FTile Tile) 
		{
			return CheckBiomeAndRoomTypeFromLevelName(Tile.LevelName, Biome, RoomType);
		});

	// Pick a random room from the FilteredRoomTiles
	int RandomIndex = FMath::RandRange(0, FilteredRoomTiles.Num() - 1);
	FTile RandomTile = FilteredRoomTiles[RandomIndex];

	// Return ID of TilesPool[TileType::LEVELROOM] 
	//return RoomTiles->IndexOfByKey(RandomTile);
	return FTile(TileType::PT_LEVELROOM,
		RoomType != RoomType::START_X && RoomType != RoomType::START_Y ? false : true,
		RandomTile.LevelName, RandomTile.StreamingLevelID);
}


void USLevelManager::InitializeIslandLevel(FIslandLevel& level) 
{
	// Generate Path of RoomType
	TArray<RoomType> RoomPath = {};

	// Start
	RoomType PreviousRoomType = FMath::RandRange(0, 1) == 0 ? RoomType::START_Y : RoomType::START_X;
	RoomPath.Add(PreviousRoomType);

	// In between Rooms
	for (int i = 1; i < LEVELROOMS_COUNT - 1; i++) {
		RoomType NextRoomType = GetRandomRoomType(PreviousRoomType);
		RoomPath.Add(NextRoomType);
		PreviousRoomType = NextRoomType;
	}

	// Create End Room
	RoomPath.Add(GetRandomEndType(PreviousRoomType));

	// Initialize Grid
	InitializeGrid(level.Grid, RoomPath, level.Biome);
}

// Return Coordinates of the next Room in path depending on the current RoomType and coordinates
inline FVector2D GetNextCoordinate(RoomType RoomType, FVector2D Coordinates)
{
	if (RoomType == RoomType::LEFTTOBACK || RoomType == RoomType::BACKTOFRONT || RoomType == RoomType::START_X) return Coordinates + FVector2D(0, 2);
	if (RoomType == RoomType::FRONTTORIGHT || RoomType == RoomType::RIGHTTOLEFT || RoomType == RoomType::START_Y) return Coordinates + FVector2D(2, 0);
	else return Coordinates;
}

// Return Coordinates of the Playable Transition between current and next Room depending on the current RoomType and coordinates
inline FVector2D GetTransitionCoordinate(RoomType RoomType, FVector2D Coordinates)
{
	if (RoomType == RoomType::LEFTTOBACK || RoomType == RoomType::BACKTOFRONT || RoomType == RoomType::START_X) return Coordinates + FVector2D(0, 1);
	if (RoomType == RoomType::FRONTTORIGHT || RoomType == RoomType::RIGHTTOLEFT || RoomType == RoomType::START_Y) return Coordinates + FVector2D(1, 0);
	else return Coordinates;
}

void USLevelManager::InitializeGrid(TArray<TArray<FTile>>& Grid, TArray<RoomType> RoomPath, BiomeType Biome) 
{
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
		RoomType NextRoomType = RoomPath[i + 1];
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

float USLevelManager::GetTileLocationFromGridCoordinate(int id)
{
	if (id % 2 == 0)
		return (id / 2) * (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE) + 0.5 * LANDSCAPE_GRID_SIZE;
	else
		return ((id / 2) + 1) * LANDSCAPE_GRID_SIZE + (id / 2) * TRANSITION_GRID_SIZE + 0.5 * TRANSITION_GRID_SIZE;
}

FTransform USLevelManager::GetTileTransformFromGridCoordinates(int idx, int idy)
{
	FTransform Transform;

	// Location (Swap axes because of unreal axis)
	float WorldLocationX = GetTileLocationFromGridCoordinate(idy);
	float WorldLocationY = GetTileLocationFromGridCoordinate(idx);

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

void USLevelManager::LoadLevelTiles()
{
	if (!CheckIslandIDValid())
		return;

	TilesToUpdate.Empty();
	TilesShownNum = 0;
	OnLevelBegin = true;

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
			LevelInstance->LevelTransform = FTransform(GetTileTransformFromGridCoordinates(idx, idy));

			if (!CurrentIslandLevel.GridLoaded)
			{
				Tile.GridID = StreamingLevelID;
				StreamingLevelID++;
			}
			else
			{
				Tile.FirstTimeShown = true;
				Tile.PatrollerPaths.Empty();
				Tile.LevelLights.Empty();
			}

			LevelInstance->SetShouldBeLoaded(true);
			if (ShouldTileBeVisible(FIntPoint(idx, idy), CurrentPlayerGridCoord))
			{
				TilesToUpdate.Add(&Tile);
				LevelInstance->SetShouldBeVisible(true);
				LevelInstance->OnLevelShown.AddDynamic(this, &USLevelManager::OnTileShown);
			}
		}
	}

	CurrentIslandLevel.GridLoaded = true;
}


void USLevelManager::OnTileShown()
{
	TilesShownNum++;
	// Wait until all tiles are shown (tiles of any type in the TilesToUpdate list)
	int MaxTiles = OnLevelBegin ? 23 : 10;
	UE_LOG(LogTemp, Warning, TEXT("RECEIVE UNIQUE DELEGATE : %d / %d"), TilesShownNum, MaxTiles);
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
	bool NeedToSearchForActors = false;
	for (FTile* Tile : LevelRoomTiles)
	{
		if (Tile->FirstTimeShown)
		{
			NeedToSearchForActors = true;
			break;
		}
	}

	// If there is at least one, find all patrol paths on the map
	TArray<AActor*> PatrolPaths;
	TArray<AActor*> LevelLights;
	TArray<AActor*> Cameras;
	if (NeedToSearchForActors)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASPatrolPath::StaticClass(), PatrolPaths);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASLevelLight::StaticClass(), LevelLights);
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASCamera::StaticClass(), Cameras);
	}

	// Update Tiles
	for (FTile* Tile : LevelRoomTiles)
	{
		if (Tile->FirstTimeShown) 
		{
			Tile->FillActors(PatrolPaths, Cameras, LevelLights, GetWorld()->GetStreamingLevels());
		}
		Tile->OnTileShown();
	}

	// Event for grid update
	if (OnLevelBegin)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("UpdateGridVisibility"));
		GetWorld()->GetTimerManager().SetTimer(GridTimerHandle, TimerDel, 5.f, true /* loop */);

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


}

void USLevelManager::GetGridCoordFromWorldLocation(FIntPoint& TileCoord, const FVector& WorldLocation)
{
	TileCoord.X = ((int)WorldLocation.X / (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE)) * 2;
	TileCoord.Y = ((int)WorldLocation.Y / (LANDSCAPE_GRID_SIZE + TRANSITION_GRID_SIZE)) * 2;
}

void USLevelManager::UpdateGridVisibility()
{
	UE_LOG(LogTemp, Warning, TEXT("UPDATE ___________________"));

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

	// Reset states
	TilesToUpdate.Empty();
	TilesShownNum = 0;
	OnLevelBegin = false;

	FIslandLevel& CurrentIslandLevel = Islands[CurrentIslandID];
	const TArray<ULevelStreaming*>& StreamedLevels = GetWorld()->GetStreamingLevels();
	for (size_t idx = 0; idx < CurrentIslandLevel.Grid.Num(); idx++)
	{
		for (size_t idy = 0; idy < CurrentIslandLevel.Grid.Num(); idy++)
		{
			FTile& Tile = CurrentIslandLevel.Grid[idx][idy];
			ULevelStreaming* StreamingLevel = StreamedLevels[Tile.GridID];
			bool ShouldBeVisible = ShouldTileBeVisible(FIntPoint(idx, idy), CurrentPlayerGridCoord, 2);

			// This is needed if we want to disable some state (for example show vision cone) if enemies are not in the playe tile
			Tile.SetPlayerTile(CurrentPlayerGridCoord == FIntPoint(idy, idx));

			if (ShouldBeVisible && StreamingLevel->GetShouldBeVisibleFlag())
			{
				continue;
			}
			else if (ShouldBeVisible)
			{
				StreamingLevel->SetShouldBeVisible(true);
				TilesToUpdate.Add(&Tile);
				UE_LOG(LogTemp, Warning, TEXT("ADD UNIQUE DELEGATE"));
				StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &USLevelManager::OnTileShown);
			}
			else
			{
				StreamingLevel->SetShouldBeVisible(false);
			}
		}
	}
}


void USLevelManager::CompleteRoom(FVector TriggerWorldLocation)
{
	FIntPoint gridCoord;
	GetGridCoordFromWorldLocation(gridCoord, TriggerWorldLocation);
	FTile& Tile = Islands[CurrentIslandID].Grid[gridCoord.Y][gridCoord.X];
	Tile.IsCompleted = true;

	for (auto Light : Tile.LevelLights)
	{
		Light->TurnOn(true);
	}

	for (auto Path : Tile.PatrollerPaths) {
		Path->Patroller->OnRoomComplete();
		Path->IsAlive = false;
	}
}

TArray<ASPatrolPath*> USLevelManager::GetPatrollersFromActorTile(AActor* Actor)
{
	check(Actor);
	FIntPoint TileCoord;
	GetGridCoordFromWorldLocation(TileCoord, Actor->GetActorLocation());
	if (CheckIslandIDValid())
	{
		return Islands[CurrentIslandID].Grid[TileCoord.Y][TileCoord.X].PatrollerPaths;
	}
	else
	{
		return TArray<ASPatrolPath*>();
	}
}


/////////////////////////////////////////////////////////////////////////////////////

// FTile Functions

void FTile::FillActors(TArray<AActor*> PatrollerPathActors, TArray<AActor*> CameraActors, TArray<AActor*> LevelLightActors, const TArray<ULevelStreaming*>& StreamingLevels)
{
	if (Type != TileType::PT_LEVELROOM)
		return;

	const auto StreamingLevel = StreamingLevels[GridID];
	FBox TileBox = ALevelBounds::CalculateLevelBounds(StreamingLevel->GetLoadedLevel());

	if (!TileBox.IsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FTile::FillActors] FBox Invalid."));
		return;
	}

	// Retrieve all Patroller Paths inside the tile
	for (auto Actor : PatrollerPathActors)
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

	// Retrieve all Cameras inside the tile
	for (auto Actor : CameraActors)
	{
		if (TileBox.IsInsideXY(Actor->GetActorLocation()))
		{
			auto Camera = Cast<ASCamera>(Actor);
			if (Camera)
			{
				Cameras.Add(Camera);
			}
		}
	}

	// Retrieve all LevelLight inside the tile
	for (auto Actor : LevelLightActors)
	{
		if (TileBox.IsInsideXY(Actor->GetActorLocation()))
		{
			auto LevelLight = Cast<ASLevelLight>(Actor);
			if (LevelLight)
			{
				LevelLights.Add(LevelLight);
			}
		}
	}
}

void FTile::OnTileShown()
{
	if (Type != TileType::PT_LEVELROOM)
		return;

	if (PatrollerPaths.Num() == 0 && LevelLights.Num() == 0)
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
			if (IsValid(PatrollerPath))
			{
				PatrollerPath->IsAlive = !IsCompleted;
				PatrollerPath->CreatePatroller();
			}
		}

		for (auto Light : LevelLights)
		{
			if (IsValid(Light))
			{
				Light->TurnOn(IsCompleted);
			}
		}
		FirstTimeShown = false;
	}
	else
	{
		for (auto PatrollerPath : PatrollerPaths)
		{
			if (IsValid(PatrollerPath))
			{
				UE_LOG(LogTemp, Warning, TEXT("RESET Patroller"));
				PatrollerPath->ResetPatroller();
			}
		}
	}
}


void FTile::SetPlayerTile(bool IsPlayerTile)
{
	for (auto PatrollerPath : PatrollerPaths)
	{
		if (IsValid(PatrollerPath))
		{
			PatrollerPath->Patroller->InsidePlayerTile = IsPlayerTile;
			PatrollerPath->Patroller->OnPlayerTileChanged();
		}
	}

	for (auto Camera : Cameras)
	{
		if (IsValid(Camera))
		{
			Camera->InsidePlayerTile = IsPlayerTile;
			Camera->OnPlayerTileChanged();
		}
	}
}
