// Fill out your copyright notice in the Description page of Project Settings.

#include "SPatroller.h"
#include "Components/SEnemyComponent.h"

// Sets default values
ASPatroller::ASPatroller()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyComp = CreateDefaultSubobject<USEnemyComponent>("EnemyComponent");
	
}

void ASPatroller::OnTileReload()
{

}

// Called when the game starts or when spawned
void ASPatroller::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPatroller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASPatroller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

USEnemyComponent* ASPatroller::GetEnemyComp()
{
	return EnemyComp;
}

bool ASPatroller::IsInsidePlayerTile()
{
	return InsidePlayerTile;
}



