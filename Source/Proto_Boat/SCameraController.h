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

	UFUNCTION(BlueprintImplementableEvent, Category = "SCameraController")
	void OnReachMaxAngle();

protected:
	UPROPERTY(BlueprintReadWrite)
	FVector OriginDirection;
	
	UPROPERTY(BlueprintReadOnly)
	FVector CurrentDirection;

	UPROPERTY(BlueprintReadOnly)
	float CurrentAngle;

private:
	float Sign = 1.0f;
};
