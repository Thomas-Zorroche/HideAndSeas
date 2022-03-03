// Fill out your copyright notice in the Description page of Project Settings.

#include "SCameraController.h"
#include "Components/SEnemyComponent.h"

ASCameraController::ASCameraController()
{

}

void ASCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDirection(DeltaTime);
}

void ASCameraController::UpdateDirection(float DeltaTime)
{
	if (!PawnFixe && State == AIState::PATROL)
	{
		CurrentAngle += (DeltaTime * EnemyComp->PatrolWalkSpeed) * Sign;

		if (FMath::Abs(CurrentAngle) > EnemyComp->CameraPatrolAngle)
		{
			CurrentAngle = EnemyComp->CameraPatrolAngle * Sign;
			Sign *= -1.0f;
			PawnFixe = true;
			OnReachMaxAngle();
		}

		CurrentDirection = OriginDirection.RotateAngleAxis(CurrentAngle, FVector(0.0f, 0.0f, 1.0f));
	}
}

void ASCameraController::InitializeDirection(const FVector& Dir)
{
	OriginDirection = Dir;
	CurrentDirection = Dir;
}
