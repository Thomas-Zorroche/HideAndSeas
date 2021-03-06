// Fill out your copyright notice in the Description page of Project Settings.

#include "SCamera.h"
#include "Components/SEnemyComponent.h"

// Sets default values
ASCamera::ASCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	EnemyComp = CreateDefaultSubobject<USEnemyComponent>("EnemyComponent");
	EnemyComp->InitializeCamera();
}

// Called when the game starts or when spawned
void ASCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void ASCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

USEnemyComponent* ASCamera::GetEnemyComp()
{
	return EnemyComp;
}

bool ASCamera::IsInsidePlayerTile()
{
	return InsidePlayerTile;
}
