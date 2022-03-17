// Fill out your copyright notice in the Description page of Project Settings.


#include "SEnemyController.h"
#include "STopDownCharacter.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/SEnemyComponent.h"
#include "IsEnemy.h"


ASEnemyController::ASEnemyController()
{
	AIPerception = GetAIPerceptionComponent();
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerception);
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerception->ConfigureSense(*SightConfig);

	EnemyComp = nullptr;
}

// Called every frame
void ASEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (State == AIState::SEARCH || State == AIState::PATROL)
		DecreaseAlertLevel(DeltaTime);
	else if (State == AIState::ALERT)
		IncreaseAlertLevel(DeltaTime);
}

void ASEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!InPawn)
		return;

	OnEnemyComponentChanged();

	if (AIPerception)
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ASEnemyController::ActorsPerceptionUpdated);
	else
		UE_LOG(LogTemp, Error, TEXT("No AIPerception defined!"));
}

void ASEnemyController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ASEnemyController::OnEnemyComponentChanged()
{
	IIsEnemy* SightInterface = Cast<IIsEnemy>(GetPawn());
	if (SightInterface)
	{
		EnemyComp = SightInterface->GetEnemyComp();
	}

	UpdateSightConfig();
}

void ASEnemyController::ActorsPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ASTopDownCharacter* Player = Cast<ASTopDownCharacter>(Actor);
	if (!IsValid(Player))
	{
		return;
	}

	if (Player->DebugMode)
	{
		return;
	}
	
	Distraction = Cast<ASSpellDistraction>(Actor);
	if (Distraction) {
		if (State == AIState::PATROL || State == AIState::SEARCH) {
			SetAIState(AIState::DISTRACTED);
		}
		return;
	}

	switch (State)
	{
	case AIState::PATROL:
		if (Player->isVisible)
		{
			SetAIState(AIState::ALERT);
		}
		else
		{
			PlayerHideInsideCone = true;
		}
		break;
	case AIState::DISTRACTED:
		if (Player->isVisible)
			SetAIState(AIState::ALERT);
		break;
	case AIState::SEARCH:
		if (Player->isVisible)
		{
			SetAIState(AIState::ALERT);
		}
		else
		{
			PlayerHideInsideCone = true;
		}
		break;
	case AIState::ALERT:	SetAIState(AIState::SEARCH); break;
	case AIState::ATTACK:	break;
	}

	UE_LOG(LogTemp, Error, TEXT("NEW STATE: %d"), (uint8)State);
}

void ASEnemyController::SetAlertLevel(const float NewAlertLevel) {
	AlertLevel = NewAlertLevel;
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
		{
			Blackboard->SetValueAsObject("TargetActor", PlayerController);
		}

		OnAttack();
	}

	// Set State to Blackboard
	if (Blackboard)
	{
		Blackboard->SetValueAsEnum("State", static_cast<uint8>(State));
	}

	switch (State)
	{
		case AIState::PATROL:		DebugStateLabel = "PATROL"; OnPatrol();  break;
		case AIState::SEARCH:		DebugStateLabel = "SEARCH"; OnSearch(); break;
		case AIState::ALERT:		DebugStateLabel = "ALERT"; OnAlert(); break;
		case AIState::ATTACK:		DebugStateLabel = "ATTACK"; OnAttack(); break;
		case AIState::DISTRACTED:	DebugStateLabel = "DISTRACTED"; OnDistract(); break;
	}

	OnDebugStateLabelChanged(DebugStateLabel);
}

void ASEnemyController::IncreaseAlertLevel(float DeltaTime)
{
	if (AlertLevel >= 1.0f)
	{
		SetAIState(AIState::ATTACK);
		return;
	}

	auto PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	float DistanceToPlayer = 1.0f;

	// If the player is invisible in front of the AI in alert mode
	ASTopDownCharacter* player = nullptr;
	if (PlayerCharacter)
		player = Cast<ASTopDownCharacter>(PlayerCharacter);
	if (player != NULL && !player->isVisible) 
	{
		PlayerHideInsideCone = true;
		SetAIState(AIState::SEARCH);
		return;
	}

	if (PlayerCharacter)
	{
		// DISTANCE ONLY IN XY
		DistanceToPlayer = FVector::DistSquaredXY(GetPawn()->GetActorLocation(), PlayerCharacter->GetActorLocation());
		if (DistanceToPlayer > EnemyComp->SightRadius * EnemyComp->SightRadius * 1.25f)
		{
			SetAIState(AIState::SEARCH);
			AlertLevel = 0.0f;
			return;
		}

		if (IsValid(EnemyComp))
		{
			DistanceToPlayer = DistanceToPlayer / (EnemyComp->SightRadius * EnemyComp->SightRadius * 1.1f);
		}
		DistanceToPlayer = FMath::Clamp(DistanceToPlayer, 0.0f, 1.0f);
	}

	float DistanceFactor = 1.0f - FMath::Pow(DistanceToPlayer, 3);
	
	if (IsValid(EnemyComp))
	{
		AlertLevel += EnemyComp->AlertSpeed * DeltaTime * DistanceFactor;
	}
	AlertLevel = FMath::Clamp(AlertLevel, 0.0f, 1.0f);
	

}

void ASEnemyController::DecreaseAlertLevel(float DeltaTime)
{
	if (AlertLevel == 0.0f)
		return;
	
	if (IsValid(EnemyComp))
	{
		AlertLevel -= EnemyComp->AlertSpeed * DeltaTime;
	}
	AlertLevel = FMath::Clamp(AlertLevel, 0.0f, 1.0f);
}


void ASEnemyController::UpdateSightConfig()
{
	FAISenseID Id = UAISense::GetSenseID(UAISense_Sight::StaticClass());

	if (!Id.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong Sense ID"));
		return;
	}

	auto Config = AIPerception->GetSenseConfig(Id);
	if (Config == nullptr)
		return;

	auto ConfigSight = Cast<UAISenseConfig_Sight>(Config);

	ConfigSight->SightRadius = EnemyComp->SightRadius;
	ConfigSight->LoseSightRadius = EnemyComp->SightRadius * 1.1f;
	ConfigSight->PeripheralVisionAngleDegrees = EnemyComp->SightAngle;

	AIPerception->RequestStimuliListenerUpdate();
}


void ASEnemyController::OnPlayerHideEnd()
{
	if (PlayerHideInsideCone && (State == AIState::PATROL || State == AIState::SEARCH))
	{
		// Check if player is still inside cone
		TArray<AActor*> OutActors;
		AIPerception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), OutActors);
		auto PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

		for (auto Actor : OutActors)
		{
			if (IsValid(Actor) && Actor == PlayerCharacter)
			{
				SetAIState(AIState::ALERT);
				return;
			}
		}
	}
	else
	{
		PlayerHideInsideCone = false;
	}
}
