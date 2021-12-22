// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SEnemy.h"
#include "SEnemyController.generated.h"


UENUM(BlueprintType)
enum class AIState : uint8 {
	PATROL = 0 UMETA(DisplayName = "PATROL"),
	SEARCH = 1 UMETA(DisplayName = "SEARCH"),
	ALERT  = 2 UMETA(DisplayName = "ALERT"),
	ATTACK = 3 UMETA(DisplayName = "ATTACK"),
};

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API ASEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SEnemyController")
	void LinkBehaviorTree();

	virtual void SetPawn(APawn* InPawn) override;

private:
	UFUNCTION(BlueprintCallable)
	void OnPerceptionUpdated();

	UFUNCTION(BlueprintCallable)
	void SetAIState(AIState NewState);

	void IncreaseAlertLevel(float DeltaTime);
	void DecreaseAlertLevel(float DeltaTime);


private:
	// Value between 0.0 and 1.0. 1.0 being fully alerted --> Attack State
	float AlertLevel = 0.0f;

	// Speed at which the enemy is alerted
	float AlertSpeed = 0.2f;

	// State of the enemy
	AIState State = AIState::PATROL;

	// Controlled Enemy
	ASEnemy* ControlledEnemy;
};
