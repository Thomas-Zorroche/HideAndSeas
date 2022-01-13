// Fill out your copyright notice in the Description page of Project Settings.


#include "SDistribution.h"
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/GameplayStatics.h" 
#include "GameManager.h"
#include "ProceduralLevels/SIslandLevel.h"
#include "SIsland.h"

// Sets default values
ASDistribution::ASDistribution()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	RootComponent = Box;
	BoxSize = Box->GetScaledBoxExtent();
}

void ASDistribution::Destroyed()
{
	Super::Destroy();

	// Destroy previous actors in the world
	for (auto& Actor : ActorsInWorld)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	ActorsInWorld.Empty();
}


// Called when the game starts or when spawned
void ASDistribution::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
void ASDistribution::UpdateEditor()
{
	Reset();

	TArray<FVector> Dummy;
	GenerateAllActors(Dummy);
}

void ASDistribution::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	UE_LOG(LogTemp, Warning, TEXT("Prop Changed: %s"), *PropertyName.ToString());
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, ActorsToSpawnData) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(FActorToSpawnData, Count) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(FActorToSpawnData, MinDistance) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(FActorToSpawnData, Class) ||
		PropertyName == "X" || PropertyName == "Y" || PropertyName == "Z" ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, BoxSize) || 
		PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, ZDistribution))
	{
		UpdateEditor();
	}
}
#endif

void ASDistribution::Reset()
{
	// Update Box Extent Scale
	Box->SetBoxExtent(BoxSize);

	// Destroy previous actors in the world
	for (const auto& Actor : ActorsInWorld)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	ActorsInWorld.Empty();

	if (ActorsToSpawnData.Num() == 0)
	{
		return;
	}
}

// We already generate random location, we can retrieve data from GM
void ASDistribution::GenerateActorsFromGameManager()
{
	Reset();

	auto GameManager = Cast<UGameManager>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameManager->HasIslandLevels())
	{
		auto IslandLevels = GameManager->GetIslandLevels();
		GenerateIslands(IslandLevels);
		// We generate other actors randomly
		GenerateOthersActors();
	}
}

/*
* First time we launch the game, we have to create randomly ALL actors location
* We have to return the locations of islands
*/ 
TArray<FVector> ASDistribution::GenerateActorsRandomly()
{
	Reset();

	TArray<FVector> IslandLocations;
	GenerateAllActors(IslandLocations);
	return IslandLocations;
}

void ASDistribution::GenerateAllActors(TArray<FVector>& IslandLocations)
{
	for (const auto& ActorData : ActorsToSpawnData)
	{
		if (ActorData.Class)
		{
			auto Island = Cast<ASIsland>(ActorData.Class);
			if (Island)
				IslandLocations = SpawnActorsRandomly(ActorData);
			else
				SpawnActorsRandomly(ActorData);
		}
	}
}

void ASDistribution::GenerateIslands(const TArray<USIslandLevel*>& IslandLevels)
{
	for (auto Island : IslandLevels)
	{
		SpawnActor(Island->GetWorldPosition(), IslandClass);
	}
}

void ASDistribution::GenerateOthersActors()
{
	for (const auto& ActorData : ActorsToSpawnData)
	{
		if (ActorData.Class)
		{
			FVector SpawnLocation(0.0f, 0.0f, 0.0f);
			bool SpawnLocationValid = GenerateRandomCoordinatesInsideBox(SpawnLocation, ActorData);

			auto Island = Cast<ASIsland>(ActorData.Class);
			if (Island)
				continue;

			SpawnActorsRandomly(ActorData);
		}
	}
}

TArray<FVector> ASDistribution::SpawnActorsRandomly(const FActorToSpawnData& ActorData)
{
	int SpawnActorCount = 0;
	TArray<FVector> Locations;
	for (size_t i = 0; i < ActorData.Count; i++)
	{
		FVector SpawnLocation(0.0f, 0.0f, 0.0f);
		bool SpawnLocationValid = GenerateRandomCoordinatesInsideBox(SpawnLocation, ActorData);
		if (!SpawnLocationValid)
		{
			UE_LOG(LogTemp, Error, TEXT("Spawn Location is not valid!"));
			continue;
		}
		Locations.Add(SpawnLocation);

		if (!ZDistribution)
			SpawnLocation.Z = GetActorLocation().Z;

		if (SpawnActor(SpawnLocation, ActorData.Class))
			SpawnActorCount++;
	}

	if (SpawnActorCount != ActorData.Count)
	{
		UE_LOG(LogTemp, Error, TEXT("Only %i Actors have been created but %i were asked!"), SpawnActorCount, ActorData.Count);
	}

	return Locations;
}

bool ASDistribution::SpawnActor(const FVector& SpawnLocation, TSubclassOf<AActor> Class)
{
	// Create new Actor
	AActor* Actor = GetWorld()->SpawnActor<AActor>(Class, SpawnLocation, GetActorRotation());
	if (Actor)
	{
		Actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		ActorsInWorld.Add(Actor);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn Actor!"));
		return false;
	}
}

// Must be used only at the beginning of the game
bool ASDistribution::GenerateRandomCoordinatesInsideBox(FVector& Location, const FActorToSpawnData& ActorData)
{
	bool SpawnLocationValid = true;
	int CurrentIteration = 0;
	do
	{
		Location = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), Box->GetScaledBoxExtent());
		SpawnLocationValid = true;
		// Test if location is far enough from the others
		for (const auto& Actor : ActorsInWorld)
		{
			float Distance = 0.0f;
			if (ZDistribution)
				Distance = FVector::DistSquared(Actor->GetActorLocation(), Location);
			else
				Distance = FVector::DistSquared2D(Actor->GetActorLocation(), Location);

			if (abs(Distance) < ActorData.MinDistance * ActorData.MinDistance)
			{
				SpawnLocationValid = false;
				break;
			}
		}

		CurrentIteration++;
		if (CurrentIteration > MAX_ITERATIONS)
		{
			break;
		}

	} while (!SpawnLocationValid);

	if (CurrentIteration > MIN_ITERATION_WARNING)
		UE_LOG(LogTemp, Warning, TEXT("Find Location with %i iters"), CurrentIteration);

	return SpawnLocationValid;
}



