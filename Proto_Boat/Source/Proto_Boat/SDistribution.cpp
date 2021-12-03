// Fill out your copyright notice in the Description page of Project Settings.


#include "SDistribution.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
ASDistribution::ASDistribution()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	RootComponent = Box;
	BoxSize = Box->GetScaledBoxExtent();

	Update();
}

void ASDistribution::Destroyed()
{
	Super::Destroy();

	// Destroy previous actors in the world
	for (const auto& Actor : ActorsInWorld)
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
		Update();
	}
}
#endif

void ASDistribution::Update()
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

	// For each class, spawn N Actors
	for (const auto& ActorData : ActorsToSpawnData)
	{
		if (ActorData.Class)
			SpawnActors(ActorData);
	}
}

void ASDistribution::SpawnActors(FActorToSpawnData ActorData)
{
	int SpawnActorCount = 0;
	for (size_t i = 0; i < ActorData.Count; i++)
	{
		auto SpawnLoc = FVector(0.0f, 0.0f, 0.0f);
		bool SpawnLocationValid = true;
		int CurrentIteration = 0;
		do
		{
			SpawnLoc = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), Box->GetScaledBoxExtent());
			SpawnLocationValid = true;
			// Test if location is far enough from the others
			for (const auto& Actor : ActorsInWorld)
			{
				float Distance = 0.0f;
				if (ZDistribution)
					Distance = FVector::DistSquared(Actor->GetActorLocation(), SpawnLoc);
				else
					Distance = FVector::DistSquared2D(Actor->GetActorLocation(), SpawnLoc);

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

		if (SpawnLocationValid)
		{
			if (CurrentIteration > MIN_ITERATION_WARNING)
				UE_LOG(LogTemp, Warning, TEXT("Find Location with %i iters"), CurrentIteration);

			if (!ZDistribution)
			{
				SpawnLoc.Z = GetActorLocation().Z;
			}

			// Create new Actor
			AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorData.Class, SpawnLoc, GetActorRotation());
			if (Actor)
			{
				Actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				SpawnActorCount++;
				ActorsInWorld.Add(Actor);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot spawn Actor!"));
			}
		}
	}

	if (SpawnActorCount != ActorData.Count)
	{
		UE_LOG(LogTemp, Error, TEXT("Only %i Actors have been created but %i were asked!"), SpawnActorCount, ActorData.Count);
	}

}



