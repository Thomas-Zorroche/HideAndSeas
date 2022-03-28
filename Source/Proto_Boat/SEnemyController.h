// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SSpellDistraction.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "SEnemyController.generated.h"


UENUM(BlueprintType)
enum class AIState : uint8 {
	PATROL = 0 UMETA(DisplayName = "PATROL"),
	SEARCH = 1 UMETA(DisplayName = "SEARCH"),
	ALERT  = 2 UMETA(DisplayName = "ALERT"),
	ATTACK = 3 UMETA(DisplayName = "ATTACK"),
	DISTRACTED = 4 UMETA(DisplayName = "DISTRACTED"),
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

	UFUNCTION(BlueprintCallable)
	void ActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnDebugStateLabelChanged(const FString& debugStateLabel);

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnAlert();

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnSearch();

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnPatrol();

	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemyController")
	void OnDistract();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SEnemyController")
	void OnPlayerCaught();

	UFUNCTION(BlueprintCallable)
	void SetAlertLevel(const float NewAlertLevel);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintCallable)
	void OnEnemyComponentChanged();

	UFUNCTION(BlueprintCallable)
	void OnPlayerHideEnd();

	virtual bool CanBeDistracted();

private:
	UFUNCTION(BlueprintCallable)
	void SetAIState(AIState NewState);

	void IncreaseAlertLevel(float DeltaTime);
	void DecreaseAlertLevel(float DeltaTime);

	void UpdateSightConfig();

public:
	UPROPERTY(BlueprintReadWrite)
	bool PawnFixe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ASSpellDistraction* Distraction;

	UPROPERTY(BlueprintReadOnly)
	FVector DistractionLocation;


	// State of the enemy
	UPROPERTY(BlueprintReadOnly)
	AIState State = AIState::PATROL;

protected:
	// Value between 0.0 and 1.0. 1.0 being fully alerted --> Attack State
	UPROPERTY(BlueprintReadOnly)
	float AlertLevel = 0.0f;

	class USEnemyComponent* EnemyComp;


private:
	UAIPerceptionComponent* AIPerception;
	UAISenseConfig_Sight* SightConfig;

	FString DebugStateLabel = "PATROL";

	bool PlayerHideInsideCone = false;
};
