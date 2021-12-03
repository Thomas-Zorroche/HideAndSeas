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

	UpdateCount();

	UE_LOG(LogTemp, Warning, TEXT("RECOMPILE with %i components"), ActorsInWorld.Num());
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
	//UE_LOG(LogTemp, Warning, TEXT("Prop Changed: %s"), *PropertyName.ToString());
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, Count) || 
		PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, MinDistanceActors) ||
		PropertyName == "X" || PropertyName == "Y" || PropertyName == "Z" ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, BoxSize) || 
		PropertyName == GET_MEMBER_NAME_CHECKED(ASDistribution, ZDistribution))
	{
		UpdateCount();
	}

}
#endif

void ASDistribution::UpdateCount()
{
	if (!ActorToSpawn)
	{
		return;
	}
	
	// Destroy previous actors in the world
	for (const auto& Actor : ActorsInWorld)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	ActorsInWorld.Empty();

	// Update Box Extent Scale
	Box->SetBoxExtent(BoxSize);

	for (size_t i = 0; i < Count; i++)
	{
		auto SpawnLoc = FVector(0.0f, 0.0f, 0.0f);
		bool SpawnLocationValid = true;
		int MaxIterations = 100;
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

				if (abs(Distance) < MinDistanceActors * MinDistanceActors)
				{
					SpawnLocationValid = false;
					break;
				}
			}

			CurrentIteration++;
			if (CurrentIteration > MaxIterations)
			{
				UE_LOG(LogTemp, Error, TEXT("Max iterations reached for ASDistribution::UpdateCount"));
				break;
			}

		} while (!SpawnLocationValid);

		if (SpawnLocationValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("Find Location with %i iters"), CurrentIteration);

			if (!ZDistribution)
			{
				SpawnLoc.Z = GetActorLocation().Z;
			}

			// Create new component
			AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnLoc, GetActorRotation());
			Actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			ActorsInWorld.Add(Actor);
		}
	}
}



