// Fill out your copyright notice in the Description page of Project Settings.


#include "SEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "Kismet/GameplayStatics.h" 


// Called every frame
void ASEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (State == AIState::SEARCH)
		DecreaseAlertLevel(DeltaTime);
	else if (State == AIState::ALERT)
		IncreaseAlertLevel(DeltaTime);
}

void ASEnemyController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	
	if (!InPawn)
		return;

	auto ControlledPawn = Cast<ASEnemy>(GetPawn());
	if (ControlledPawn)
		ControlledEnemy = ControlledPawn;
	else
		UE_LOG(LogTemp, Error, TEXT("Pawn must be a child class of ASEnemy!"));
}

void ASEnemyController::OnPerceptionUpdated()
{
	switch (State)
	{
		case AIState::PATROL: SetAIState(AIState::ALERT); break;
		case AIState::SEARCH: SetAIState(AIState::ALERT); break;
		case AIState::ALERT:  SetAIState(AIState::SEARCH); break;
		case AIState::ATTACK: break;
	}
}

void ASEnemyController::SetAIState(AIState NewState)
{
	if (NewState == State)
		return;

	State = NewState;

	if (State == AIState::ATTACK)
	{
		// We need to set TargetActor in the Blackboard
		auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (Blackboard && PlayerController)
			Blackboard->SetValueAsObject("TargetActor", PlayerController);
	}

	// Set State to Blackboard
	if (Blackboard)
		Blackboard->SetValueAsEnum("State", static_cast<uint8>(State));

	if (ControlledEnemy)
	{
		switch (State)
		{
		case AIState::PATROL: ControlledEnemy->UpdateDebugStateLabel("PATROL"); break;
		case AIState::SEARCH: ControlledEnemy->UpdateDebugStateLabel("SEARCH"); break;
		case AIState::ALERT:  ControlledEnemy->UpdateDebugStateLabel("ALERT"); break;
		case AIState::ATTACK: ControlledEnemy->UpdateDebugStateLabel("ATTACK"); break;
		}
	}
		
}

void ASEnemyController::IncreaseAlertLevel(float DeltaTime)
{
	if (AlertLevel == 1.0f)
	{
		SetAIState(AIState::ATTACK);
		return;
	}

	AlertLevel += AlertSpeed * DeltaTime;
	AlertLevel = FMath::Clamp(AlertLevel, 0.0f, 1.0f);

	if (ControlledEnemy)
		ControlledEnemy->UpdateLightLevel(AlertLevel);
}

void ASEnemyController::DecreaseAlertLevel(float DeltaTime)
{
	if (AlertLevel == 0.0f)
		return;

	AlertLevel -= AlertSpeed * DeltaTime;
	AlertLevel = FMath::Clamp(AlertLevel, 0.0f, 1.0f);

	if (ControlledEnemy)
		ControlledEnemy->UpdateLightLevel(AlertLevel);
}
