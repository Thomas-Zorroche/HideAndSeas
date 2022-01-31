// Fill out your copyright notice in the Description page of Project Settings.


#include "SPatrolPath.h"
#include "Components/SEnemyComponent.h"
#include "SPatroller.h"
#include "SPatrollerController.h"


const int ASPatrolPath::MARKERS_COUNT_MAX = 10;

// Sets default values
ASPatrolPath::ASPatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	EnemyComp = CreateDefaultSubobject<USEnemyComponent>("EnemyComponent");
	Patroller = nullptr;
}

void ASPatrolPath::CreatePatroller()
{
	TArray<AActor*> AttachedActors;
	this->GetAttachedActors(AttachedActors);
	if (AttachedActors.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("At least 2 markers are needed for the path."));
		return;
	}

	FillMarkersLocation(AttachedActors);
	SpawnPatroller();
	OnSpawnedPatroller();
}


void ASPatrolPath::SpawnPatroller()
{
	if (MarkersLocation.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn patroller with less than 2 marker locations."));
		return; 
	}

	FActorSpawnParameters SpawnParameters;
	// Needed to spawn patroller inside his own level streaming
	SpawnParameters.Owner = this; 
	auto Actor = GetWorld()->SpawnActor<ASPatroller>(PatrollerClass, MarkersLocation[0], FRotator(EForceInit::ForceInitToZero), SpawnParameters);
	Patroller = Cast<ASPatroller>(Actor);
	if (Patroller)
	{
		Patroller->SpawnDefaultController();
		Patroller->EnemyComp = EnemyComp;
		auto EnemyController = Patroller->GetController();
		auto PatrollerController = Cast<ASPatrollerController>(EnemyController);
		if (PatrollerController)
		{
			//EnemyController->OnEnemyComponentChanged(); askip c'est appelé tout seul
			PatrollerController->MarkersLocations = MarkersLocation;
			PatrollerController->LinkBehaviorTree();
		}
	}
}

void ASPatrolPath::ResetPatroller()
{
	Patroller->SpawnDefaultController();

	auto EnemyController = Patroller->GetController();
	auto PatrollerController = Cast<ASPatrollerController>(EnemyController);
	if (PatrollerController)
	{
		//EnemyController->OnEnemyComponentChanged(); //askip c'est appelé tout seul
		PatrollerController->MarkersLocations = MarkersLocation;
		PatrollerController->LinkBehaviorTree();
	}
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
	if (MarkersLocation.Num() >= MARKERS_COUNT_MAX)
		return;

	// Create marker
	auto Marker = GetWorld()->SpawnActor<ASEmptyMarker>(ASEmptyMarker::StaticClass(), Location, FRotator());
	if (Marker)
	{
		Marker->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		Marker->SetIndex(MarkersLocation.Num());
		MarkersLocation.Add(Marker->GetActorLocation());
	}
}

void ASPatrolPath::AddMarker()
{
	AddMarkerAtLocation();
}

void ASPatrolPath::FillMarkersLocation(const TArray<AActor*>& AttachedActors)
{
	MarkersLocation.Empty();
	int index = 0;
	for (auto Child : AttachedActors)
	{
		auto Marker = Cast<ASEmptyMarker>(Child);
		if (Marker)
		{
			Marker->SetIndex(index);
			MarkersLocation.Add(Marker->GetActorLocation());
			index++;
		}
	}
}

TArray<FVector> ASPatrolPath::GetMarkersLocation() const
{
	//TArray<FVector> Locations;
	//for (auto Marker : Markers)
	//{
	//	Locations.Add(Marker->GetActorLocation());
	//}
	//return Locations;
	return MarkersLocation;
}







