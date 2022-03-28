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

	void InitializeChampi();
	
	void InitializeGolem();
	
	void InitializeCamera();

	UFUNCTION(CallInEditor, Category = "SEnemyComponent"/*, meta = (EditCondition = "IsCamera && OverrideParameters")*/)
	void ResetCamera();

public:	
	UPROPERTY()
	bool IsCamera = false;

	UPROPERTY(EditAnywhere)
	bool OverrideParameters = false;

	// How fast the enemy is alerted
	UPROPERTY(BlueprintReadOnly)
	float AlertSpeed = 1.0f;

	// How long the enemy wait when he's in SearchState
	UPROPERTY(BlueprintReadOnly)
	float SearchWait = 4.0f;

	// How long the enemy wait when he's in PatrolState at the end of a path 
	// (On a checkpoint for patrollers, at the maximum angle for cameras)
	UPROPERTY(BlueprintReadOnly)
	float PatrolWait = 4.0f;

	// Enemy's speed (Angular speed for cameras)
	UPROPERTY(BlueprintReadOnly)
	float PatrolWalkSpeed = 93.5f;

	// Enemy's speed when attacking
	UPROPERTY(BlueprintReadOnly)
	float AttackWalkSpeed = 375.5f;

	// How far the enemy can spot the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "OverrideParameters"))
	float SightRadius = 500.0f;

	// Field of view of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "OverrideParameters"))
	float SightAngle = 60.0f;

	// Patrol angle for cameras. Ignored if the pawn is not a Camera.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "IsCamera && OverrideParameters"))
	float CameraPatrolAngle = 60.0f;

};
