// Fill out your copyright notice in the Description page of Project Settings.


#include "SPatrolPath.h"
#include "Components/SEnemyComponent.h"


const int ASPatrolPath::MARKERS_COUNT_MAX = 10;

// Sets default values
ASPatrolPath::ASPatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");

	EnemyComp = CreateDefaultSubobject<USEnemyComponent>("EnemyComponent");
}

void ASPatrolPath::OnLevelLoaded_Implementation()
{
	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);
	UE_LOG(LogTemp, Warning, TEXT("Children at start: %d"), AttachedActors.Num());

	// At least two markers in the path
	if (AttachedActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADD MARKERS"));
		AddMarkerAtLocation(FVector(0, 0, 0));
		AddMarkerAtLocation(FVector(100, 0, 0));
	}
	else if (AttachedActors.Num() == 1)
		AddMarkerAtLocation(FVector(0, 0, 0));

	Update();
	OnMarkersChanged();
}


// Called every frame
void ASPatrolPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void ASPatrolPath::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	UE_LOG(LogTemp, Warning, TEXT("Prop Changed: %s"), *PropertyName.ToString());
}
#endif


void ASPatrolPath::AddMarkerAtLocation(FVector Location)
{
	if (Markers.Num() >= MARKERS_COUNT_MAX)
		return;

	// Create marker
	auto Marker = GetWorld()->SpawnActor<ASEmptyMarker>(ASEmptyMarker::StaticClass(), Location, FRotator());
	if (Marker)
	{
		Marker->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		Marker->SetIndex(Markers.Num());
		Markers.Add(Marker);
	}
}

void ASPatrolPath::AddMarker()
{
	AddMarkerAtLocation();
}

void ASPatrolPath::Update()
{
	Markers.Empty();
	int index = 0;
	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);
	
	for (auto Child : AttachedActors)
	{
		auto Marker = Cast<ASEmptyMarker>(Child);
		if (Marker)
		{
			Marker->SetIndex(index);
			Markers.Add(Marker);
			index++;
		}
	}
}

TArray<FVector> ASPatrolPath::GetMarkersLocation() const
{
	TArray<FVector> Locations;
	for (auto Marker : Markers)
	{
		Locations.Add(Marker->GetActorLocation());
	}
	return Locations;
}





