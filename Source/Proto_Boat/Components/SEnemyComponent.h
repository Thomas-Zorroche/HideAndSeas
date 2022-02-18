// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SEnemyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTO_BOAT_API USEnemyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USEnemyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// How fast the enemy is alerted
	UPROPERTY(EditAnywhere)
	float AlertSpeed = 0.2f;

	// How long the enemy wait when he's in SearchState
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SearchWait = 4.0f;

	// How long the enemy wait when he's in PatrolState at the end of a path 
	// (On a checkpoint for patrollers, at the maximum angle for cameras)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrolWait = 4.0f;

	// Enemy's speed (Angular speed for cameras)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed = 10.0f;

	// How far the enemy can spot the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SightRadius = 500.0f;

	// Field of view of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SightAngle = 60.0f;

	// Patrol angle for cameras. Ignored if the pawn is not a Camera.
	UPROPERTY(EditAnywhere)
	float CameraPatrolAngle = 60.0f;
		
};
