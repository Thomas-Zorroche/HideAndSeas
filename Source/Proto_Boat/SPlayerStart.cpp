// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerStart.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" 


// Sets default values
ASPlayerStart::ASPlayerStart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASPlayerStart::OnLevelLoaded_Implementation()
{
	OnPlayerStart();
}

// Called when the game starts or when spawned
void ASPlayerStart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPlayerStart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

