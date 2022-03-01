// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SEnemyController.h"
#include "SPatrollerController.generated.h"

/**
 * 
 */
UCLASS()
class PROTO_BOAT_API ASPatrollerController : public ASEnemyController
{
	GENERATED_BODY()
public:

	// To rotate the AI perception cone with the pawn
	//virtual FRotator GetControlRotation() const override;

	void GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const override;
	UFUNCTION(BlueprintImplementableEvent)
	void DeterminAISightPerceptionViewPoint(FVector& out_location, FRotator& out_Rotation) const;


public:

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> MarkersLocations;
};
