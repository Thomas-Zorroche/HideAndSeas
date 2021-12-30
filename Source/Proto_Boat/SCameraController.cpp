// Fill out your copyright notice in the Description page of Project Settings.


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
		CurrentAngle += (DeltaTime * RotationSpeed) * Direction;

		if (FMath::Abs(CurrentAngle) > AngleMax)
			Direction *= -1.0f;

		CurrentDirection = OriginDirection.RotateAngleAxis(CurrentAngle, FVector(0.0f, 0.0f, 1.0f));
	}
}

void ASCameraController::InitializeDirection(const FVector& Dir)
{
	OriginDirection = Dir;
	CurrentDirection = Dir;
}
