// Fill out your copyright notice in the Description page of Project Settings.


#include "SEnemyComponent.h"

// Sets default values for this component's properties
USEnemyComponent::USEnemyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void USEnemyComponent::InitializeChampi()
{
	AlertSpeed = 3.5f;
	SearchWait = 2.0f;
	PatrolWait = 3.0f;

	{   // DO NOT TOUCH THIS
		PatrolWalkSpeed = 137.0f; 
		AttackWalkSpeed = 550.0f;
	}

	if (!OverrideParameters)
	{
		SightRadius = 350.0f;
		SightAngle = 60.0f;
		CameraPatrolAngle = 0.0f;
	}
}

void USEnemyComponent::InitializeGolem()
{
	AlertSpeed = 3.5f;
	SearchWait = 2.0f;
	PatrolWait = 1.5f;

	{   // DO NOT TOUCH THIS
		PatrolWalkSpeed = 125.0f;
		AttackWalkSpeed = 500.0f;
	}

	if (!OverrideParameters)
	{
		SightRadius = 550.0f;
		SightAngle = 80.0f;
		CameraPatrolAngle = 0.0f;
	}
}

void USEnemyComponent::InitializeCamera()
{
	IsCamera = true;

	AlertSpeed = 6.5f;
	SearchWait = 0.5f;
	PatrolWait = 0.5f;
	PatrolWalkSpeed = 130.0f;

	SightRadius = 650.0f;
	SightAngle = 30.0f;
	CameraPatrolAngle = 50.0f; // TODO
}

void USEnemyComponent::ResetCamera()
{
	InitializeCamera();
}

