// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnLightLevelChanged(const float currentLightIntensity);

	UFUNCTION(BlueprintCallable)
	void ActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnDebugStateLabelChanged(const FString& debugStateLabel);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION(BlueprintCallable)
	void SetAIState(AIState NewState);

	void IncreaseAlertLevel(float DeltaTime);
	void DecreaseAlertLevel(float DeltaTime);


protected:
	// ALERT
	// Speed at which the enemy is alerted
	UPROPERTY(EditAnywhere)
	float AlertSpeed = 0.2f;
	// Value between 0.0 and 1.0. 1.0 being fully alerted --> Attack State
	UPROPERTY(BlueprintReadOnly)
	float AlertLevel = 0.0f;
	
	// AI PERCEPTION
	UPROPERTY(EditAnywhere)
	float SightRadius = 1000.0f;
	UPROPERTY(EditAnywhere)
	float SightAngle = 60.0f;

	// SPOT LIGHT
	UPROPERTY(EditAnywhere)
	float BaseLightIntensity = 500.0f;
	UPROPERTY(EditAnywhere)
	float MaxLightIntensity = 5000.0f;

	// State of the enemy
	AIState State = AIState::PATROL;

private:
	UAIPerceptionComponent* AIPerception;
	UAISenseConfig_Sight* SightConfig;

	float CurrentLightIntensity = BaseLightIntensity;
	FString DebugStateLabel = "PATROL";
};
