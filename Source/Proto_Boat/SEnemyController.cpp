// Fill out your copyright notice in the Description page of Project Settings.


#include "SEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h" 


ASEnemyController::ASEnemyController()
{
	AIPerception = GetAIPerceptionComponent();
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerception);
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	
	// Editor Variables
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = SightAngle;
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerception->ConfigureSense(*SightConfig);
}

// Called every frame
void ASEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (State == AIState::SEARCH)
		DecreaseAlertLevel(DeltaTime);
	else if (State == AIState::ALERT)
		IncreaseAlertLevel(DeltaTime);
}

void ASEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!InPawn)
		return;

	if (AIPerception)
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ASEnemyController::ActorsPerceptionUpdated);
	else
		UE_LOG(LogTemp, Error, TEXT("No AIPerception defined!"));
	
	OnLightLevelChanged(CurrentLightIntensity);
}

void ASEnemyController::OnUnPossess()
{
	Super::OnUnPossess();

}

void ASEnemyController::ActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
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

	switch (State)
	{
	case AIState::PATROL: DebugStateLabel = "PATROL"; break;
	case AIState::SEARCH: DebugStateLabel = "SEARCH"; break;
	case AIState::ALERT:  DebugStateLabel = "ALERT"; break;
	case AIState::ATTACK: DebugStateLabel = "ATTACK"; break;
	}

	OnDebugStateLabelChanged(DebugStateLabel);
}

void ASEnemyController::IncreaseAlertLevel(float DeltaTime)
{
	if (AlertLevel == 1.0f)
	{
		SetAIState(AIState::ATTACK);
		return;
	}

	auto PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	float DistanceToPlayer = 1.0f;
	if (PlayerCharacter)
	{
		// DISTANCE ONLY IN XY
		DistanceToPlayer = FVector::DistSquaredXY(GetPawn()->GetActorLocation(), PlayerCharacter->GetActorLocation());
		DistanceToPlayer = DistanceToPlayer / (SightRadius * SightRadius);
		DistanceToPlayer = FMath::Clamp(DistanceToPlayer, 0.0f, 1.0f);
	}

	float DistanceFactor = FMath::Square(1.0f - DistanceToPlayer);
	
	//UE_LOG(LogTemp, Warning, TEXT("Dst: %f"), (1.0f - DistanceToPlayer));
	AlertLevel += AlertSpeed * DeltaTime * DistanceFactor;
	AlertLevel = FMath::Clamp(AlertLevel, 0.0f, 1.0f);

	// Update Light Level
	CurrentLightIntensity = BaseLightIntensity + AlertLevel * (MaxLightIntensity - BaseLightIntensity);

	OnLightLevelChanged(CurrentLightIntensity);
}

void ASEnemyController::DecreaseAlertLevel(float DeltaTime)
{
	if (AlertLevel == 0.0f)
		return;

	AlertLevel -= AlertSpeed * DeltaTime;
	AlertLevel = FMath::Clamp(AlertLevel, 0.0f, 1.0f);

	// Update Light Level
	CurrentLightIntensity = BaseLightIntensity + AlertLevel * (MaxLightIntensity - BaseLightIntensity);
	
	OnLightLevelChanged(CurrentLightIntensity);
}