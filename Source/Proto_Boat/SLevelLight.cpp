// Fill out your copyright notice in the Description page of Project Settings.


#include "SLevelLight.h"

// Sets default values
ASLevelLight::ASLevelLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASLevelLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASLevelLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASLevelLight::TurnOn(bool TurnOn)
{
	if (TurnOn)
	{
		OnLightTurnOn();
	}
	else
	{
		OnLightTurnOff();
	}
}

