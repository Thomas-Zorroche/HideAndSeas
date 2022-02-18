// Fill out your copyright notice in the Description page of Project Settings.


#include "SPatrolPath.h"
#include "Components/SEnemyComponent.h"
#include "SPatroller.h"
#include "SPatrollerController.h"
#include "GameFramework/CharacterMovementComponent.h" 


const int ASPatrolPath::MARKERS_COUNT_MAX = 10;

// Sets default values
ASPatrolPath::ASPatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	EnemyComp = CreateDefaultSubobject<USEnemyComponent>("EnemyComponent");
	Patroller = nullptr;
	MarkerColor = FColor::MakeRandomColor();
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
	if (IsAlive)
	{
		OnSpawnedPatroller();
	}
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
	if (Patroller && IsAlive)
	{
		Patroller->SpawnDefaultController();

		Patroller->EnemyComp = EnemyComp;
		UCharacterMovementComponent * CharacterMovement = Patroller->GetCharacterMovement();
		CharacterMovement->MaxWalkSpeed = EnemyComp->Speed;

		auto EnemyController = Patroller->GetController();
		auto PatrollerController = Cast<ASPatrollerController>(EnemyController);
		if (PatrollerController)
		{
			PatrollerController->MarkersLocations = MarkersLocation;
			PatrollerController->LinkBehaviorTree();
		}
	}
}

void ASPatrolPath::ResetPatroller()
{
	if (!IsValid(Patroller))
	{
		UE_LOG(LogTemp, Error, TEXT("[ASPatrolPath::ResetPatroller] No Patroller Found."));
		return;
	}

	if (!IsAlive)
	{
		//OnSpawnedPatroller();
		return;
	}

	
	if (GetWorld())
	{
		Patroller->SpawnDefaultController();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WORLD IS NULL"));
		return;
	}
	

	auto Controller = Patroller->GetController();
	if (!Controller)
	{
		UE_LOG(LogTemp, Error, TEXT("[ASPatrolPath::ResetPatroller] No Controller Found."));
		return;
	}

	auto PatrollerController = Cast<ASPatrollerController>(Controller);
	if (!PatrollerController)
	{
		UE_LOG(LogTemp, Error, TEXT("[ASPatrolPath::ResetPatroller] No PatrollerController Found."));
		return;
	}

	PatrollerController->MarkersLocations = MarkersLocation;
	PatrollerController->LinkBehaviorTree();
	OnSpawnedPatroller();
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
	{
		return;
	}


	if (MarkerColor == FColor(0, 0, 0))
	{
		MarkerColor = FColor::MakeRandomColor();
	}

	// Create marker
	auto Marker = GetWorld()->SpawnActor<ASEmptyMarker>(ASEmptyMarker::StaticClass(), Location, FRotator());
	if (Marker)
	{
		Marker->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		Marker->SetIndex(MarkersLocation.Num());
		Marker->SetColor(MarkerColor);
		MarkersLocation.Add(Marker->GetActorLocation());
	}
}

void ASPatrolPath::AddMarker()
{
	FVector Location = GetActorLocation();
	Location.Z += 110.0f;
	AddMarkerAtLocation(Location);
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







