// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h" 
#include "SEmptyMarker.h"
#include "SPatrolPath.generated.h"

UCLASS()
class PROTO_BOAT_API ASPatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPatrolPath();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif

public:	

	UFUNCTION(CallInEditor, Category = "SPatrolPath")
	void AddMarker();

	void FillMarkersLocation(const TArray<AActor*>& AttachedActors);

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetMarkersLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SPatrolPath")
	void OnSpawnedPatroller();

	// On First Tile Shown 
	void CreatePatroller();
	// On Following  Tile Shown 
	void ResetPatroller();

private:
	void AddMarkerAtLocation(FVector Location = FVector(0, 0, 0));

	void SpawnPatroller();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USEnemyComponent* EnemyComp;

	UPROPERTY(BlueprintReadOnly)
	class ASPatroller* Patroller;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PatrollerClass;

	bool IsAlive = true;

private:
	static const int MARKERS_COUNT_MAX;

	TArray<FVector> MarkersLocation;
	
};
