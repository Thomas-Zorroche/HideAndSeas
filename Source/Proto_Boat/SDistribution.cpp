// Fill out your copyright notice in the Description page of Project Settings.


#include "SDistribution.h"
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/GameplayStatics.h" 
#include "GameManager.h"
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

	TArray<ASIsland*> Dummy;
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
	if (GameManager->LevelManager->HasIslandLevels())
	{
		auto IslandLevels = GameManager->LevelManager->GetIslandLevels();
		GenerateIslands(IslandLevels);
		// We generate other actors randomly
		GenerateOthersActors();
	}
}

/*
* First time we launch the game, we have to create randomly ALL actors location
* We have to return the locations of islands
*/ 
TArray<ASIsland*> ASDistribution::GenerateActorsRandomly()
{
	Reset();

	TArray<ASIsland*> Islands;
	GenerateAllActors(Islands);
	return Islands;
}

void ASDistribution::GenerateAllActors(TArray<ASIsland*>& Islands)
{
	Islands.Empty();
	for (const auto& ActorData : ActorsToSpawnData)
	{
		if (ActorData.Class)
		{
			bool IsIsland = (ActorData.Class == IslandClass);
			if (IsIsland)
			{

				auto SpawnedActors = SpawnActorsRandomly(ActorData);
				for (auto Actor : SpawnedActors)
				{
					auto Island = Cast<ASIsland>(Actor);
					if (Island)
						Islands.Add(Island);
				}
			}
			else
				SpawnActorsRandomly(ActorData);
		}
	}
}

void ASDistribution::GenerateIslands(const TArray<FIslandLevel>& IslandLevels)
{
	int IslandID = 0;
	for (auto IslandLevel : IslandLevels)
	{
		auto Actor = SpawnActor(IslandLevel.Transform, IslandClass);
		auto Island = Cast<ASIsland>(Actor);
		if (Island)
		{
			Island->SetID(IslandID);
		}
		IslandID++;
	}
}

void ASDistribution::GenerateOthersActors()
{
	for (const auto& ActorData : ActorsToSpawnData)
	{
		if (ActorData.Class)
		{
			bool IsIsland = (ActorData.Class == IslandClass);
			if (IsIsland)
				continue;

			FVector SpawnLocation(0.0f, 0.0f, 0.0f);
			bool SpawnLocationValid = GenerateRandomCoordinatesInsideBox(SpawnLocation, ActorData);


			SpawnActorsRandomly(ActorData);
		}
	}
}

FRotator FRandomYawRotator()
{
	const float pitch = 0.0f;
	const float yaw = FMath::FRandRange(-180.f, 180.f);
	const float roll = 0.0f;
	return FRotator(pitch, yaw, roll);
}

TArray<AActor*> ASDistribution::SpawnActorsRandomly(const FActorToSpawnData& ActorData)
{
	int SpawnActorCount = 0;
	TArray<AActor*> Actors;
	for (size_t i = 0; i < ActorData.Count; i++)
	{
		FVector SpawnLocation(0.0f, 0.0f, 0.0f);
		bool SpawnLocationValid = GenerateRandomCoordinatesInsideBox(SpawnLocation, ActorData);
		if (!SpawnLocationValid)
		{
			UE_LOG(LogTemp, Error, TEXT("Spawn Location is not valid!"));
			continue;
		}

		if (!ZDistribution)
			SpawnLocation.Z = GetActorLocation().Z;
		
		auto SpawnedActor = SpawnActor(FTransform(FRandomYawRotator(), SpawnLocation), ActorData.Class);
		if (SpawnedActor)
		{
			SpawnActorCount++;
			Actors.Add(SpawnedActor);

			auto Island = Cast<ASIsland>(SpawnedActor);
		}
	}

	if (SpawnActorCount != ActorData.Count)
	{
		UE_LOG(LogTemp, Error, TEXT("Only %i Actors have been created but %i were asked!"), SpawnActorCount, ActorData.Count);
	}

	return Actors;
}

AActor* ASDistribution::SpawnActor(const FTransform& Transform, TSubclassOf<AActor> Class)
{
	// Create new Actor
	AActor* Actor = GetWorld()->SpawnActor<AActor>(Class, Transform);
	if (Actor)
	{
		Actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		ActorsInWorld.Add(Actor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn Actor!"));
	}
	return Actor;
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



