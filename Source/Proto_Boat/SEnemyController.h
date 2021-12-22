// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SEnemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
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
	ASEnemyController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SEnemyController")
	void LinkBehaviorTree();

	//virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void ActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION(BlueprintCallable)
	void SetAIState(AIState NewState);

	void IncreaseAlertLevel(float DeltaTime);
	void DecreaseAlertLevel(float DeltaTime);


protected:
	// Speed at which the enemy is alerted
	UPROPERTY(EditAnywhere)
	float AlertSpeed = 0.2f;

	UPROPERTY(EditAnywhere)
	float SightRadius = 1000.0f;

	UPROPERTY(EditAnywhere)
	float SightAngle = 60.0f;

private:
	// Value between 0.0 and 1.0. 1.0 being fully alerted --> Attack State
	float AlertLevel = 0.0f;

	// State of the enemy
	AIState State = AIState::PATROL;

	// Controlled Enemy
	ASEnemy* ControlledEnemy;

	UAIPerceptionComponent* AIPerception;

	UAISenseConfig_Sight* SightConfig;
};
