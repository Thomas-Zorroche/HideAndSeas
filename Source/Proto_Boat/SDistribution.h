// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h" 
#include "SDistribution.generated.h"

USTRUCT()
struct FActorToSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
	
	UPROPERTY(EditAnywhere)
	int Count;
	
	UPROPERTY(EditAnywhere)
	int MinDistance;
};

/*
* Spawn Actors inside Box with the help of some data OR generate random data
*/
UCLASS()
class PROTO_BOAT_API ASDistribution : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDistribution();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;

	UFUNCTION(CallInEditor, Category="SDistribution")
	void UpdateEditor();
#endif

	UFUNCTION(BlueprintCallable, Category="SDistribution")
	void GenerateActorsFromGameManager();
	
	UFUNCTION(BlueprintCallable, Category="SDistribution")
	TArray<class ASIsland*> GenerateActorsRandomly();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual void Destroyed() override;

	void Reset();

	void GenerateAllActors(TArray<class ASIsland*>& Islands);    // All actors
	void GenerateIslands(const TArray<class FLevelIsland>& IslandLevels);	     // Only Island Actors
	void GenerateOthersActors(); // Only actors that are not Island

	TArray<AActor*> SpawnActorsRandomly(const FActorToSpawnData& ActorData);
	AActor* SpawnActor(const FVector& SpawnLocation, TSubclassOf<AActor> Class);

	bool GenerateRandomCoordinatesInsideBox(FVector& Location, const FActorToSpawnData& ActorData);

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> IslandClass;

private:
	UBoxComponent* Box;

	TArray<AActor*> ActorsInWorld;

	UPROPERTY(EditAnywhere)
	TArray< FActorToSpawnData > ActorsToSpawnData;
	
	UPROPERTY(EditAnywhere)
	FVector BoxSize;

	UPROPERTY(EditAnywhere)
	bool ZDistribution = false;

	const int MAX_ITERATIONS = 100;
	const int MIN_ITERATION_WARNING = 10; // DEBUG
};
