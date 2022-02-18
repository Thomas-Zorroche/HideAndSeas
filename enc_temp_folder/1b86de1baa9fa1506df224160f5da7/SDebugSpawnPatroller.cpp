// Fill out your copyright notice in the Description page of Project Settings.


#include "SDebugSpawnPatroller.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "./SPatrolPath.h"



// Sets default values
ASDebugSpawnPatroller::ASDebugSpawnPatroller()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDebugSpawnPatroller::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASPatrolPath::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		if (Actor)
		{
			auto PatrolPath = Cast<ASPatrolPath>(Actor);
			PatrolPath->CreatePatroller();
		}
	}
}

// Called every frame
void ASDebugSpawnPatroller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

