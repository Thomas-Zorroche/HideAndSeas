// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h" 
#include "SDistribution.generated.h"


UCLASS()
class PROTO_BOAT_API ASDistribution : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDistribution();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif
	
	UFUNCTION(CallInEditor, Category="SDistribution")
	void UpdateCount();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual void Destroyed() override;


private:
	UBoxComponent* Box;

	TArray<AActor*> ActorsInWorld;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;
	
	UPROPERTY(EditAnywhere)
	int Count = 5;

	UPROPERTY(EditAnywhere)
	float MinDistanceActors = 50.0f;

	UPROPERTY(EditAnywhere)
	FVector BoxSize;

	UPROPERTY(EditAnywhere)
	bool ZDistribution = false;
};
