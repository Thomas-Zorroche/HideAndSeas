// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SEnemyController.h"
#include "SCameraController.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API ASCameraController : public ASEnemyController
{
	GENERATED_BODY()

public:
	ASCameraController();

	virtual void Tick(float DeltaTime) override;

	void UpdateDirection(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void InitializeDirection(const FVector& Dir);

protected:
	UPROPERTY(BlueprintReadWrite)
	FVector OriginDirection;
	
	UPROPERTY(BlueprintReadOnly)
	FVector CurrentDirection;

	UPROPERTY(BlueprintReadOnly)
	float CurrentAngle;

	UPROPERTY(EditAnywhere)
	float RotationSpeed;

	UPROPERTY(EditAnywhere)
	float AngleMax;

private:
	float Direction = 1.0f;
};
