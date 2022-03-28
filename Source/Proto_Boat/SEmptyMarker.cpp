// Fill out your copyright notice in the Description page of Project Settings.


#include "SEmptyMarker.h"

// Sets default values
ASEmptyMarker::ASEmptyMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	RootComponent = SphereCollision;
}

void ASEmptyMarker::SetColor(FColor Color)
{
	SphereCollision->ShapeColor = Color;

}

// Called when the game starts or when spawned
void ASEmptyMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASEmptyMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

