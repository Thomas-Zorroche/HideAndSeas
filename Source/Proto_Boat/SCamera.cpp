// Fill out your copyright notice in the Description page of Project Settings.

#include "SCamera.h"
#include "SAISightComp.h"

// Sets default values
ASCamera::ASCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AISightComp = CreateDefaultSubobject<USAISightComp>("AISightComp");
}

// Called when the game starts or when spawned
void ASCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

USAISightComp* ASCamera::GetAISightComp()
{
	return AISightComp;
}

