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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, Category = "SPatrolPath")
	void AddMarker();

	UFUNCTION(CallInEditor, Category = "SPatrolPath")
	void Update();

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetMarkersLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SPatrolPath")
	void OnMarkersChanged();


private:
	virtual void Destroyed() override;

	void AddMarkerAtLocation(FVector Location = FVector(0, 0, 0));

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USEnemyComponent* EnemyComp;

private:
	static const int MARKERS_COUNT_MAX;

	// Spheres forming the path
	TArray<ASEmptyMarker*> Markers;
};
