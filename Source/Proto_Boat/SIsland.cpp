// Fill out your copyright notice in the Description page of Project Settings.


#include "SIsland.h"

// Sets default values
ASIsland::ASIsland()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	RootComponent = SphereCollision;
}

// Called when the game starts or when spawned
void ASIsland::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

