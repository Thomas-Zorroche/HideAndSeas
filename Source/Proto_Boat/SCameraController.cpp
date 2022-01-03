// Fill out your copyright notice in the Description page of Project Settings.

#include "SAISightComp.h"
#include "SCameraController.h"

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
	if (State == AIState::PATROL)
	{
		CurrentAngle += (DeltaTime * AISightComp->Speed) * Sign;

		if (FMath::Abs(CurrentAngle) > AISightComp->CameraPatrolAngle)
			Sign *= -1.0f;

		CurrentDirection = OriginDirection.RotateAngleAxis(CurrentAngle, FVector(0.0f, 0.0f, 1.0f));
	}
	

}

void ASCameraController::InitializeDirection(const FVector& Dir)
{
	OriginDirection = Dir;
	CurrentDirection = Dir;
}
