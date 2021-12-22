// Fill out your copyright notice in the Description page of Project Settings.


#include "SEnemy.h"

// Sets default values
ASEnemy::ASEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASEnemy::UpdateLightLevel(float AlertLevel)
{
	LightLevel = AlertLevel;
	CurrentLightIntensity = BaseLightIntensity + LightLevel * (MaxLightIntensity - BaseLightIntensity);
	OnLightLevelChanged(CurrentLightIntensity);
}

void ASEnemy::UpdateDebugStateLabel(const FString& Label)
{
	DebugStateLabel = Label;
	OnDebugStateLabelChanged(DebugStateLabel);
}

// Called when the game starts or when spawned
void ASEnemy::BeginPlay()
{
	Super::BeginPlay();
	OnDebugStateLabelChanged(DebugStateLabel);
	OnLightLevelChanged(CurrentLightIntensity);
}

// Called every frame
void ASEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

