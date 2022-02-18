// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLevelLight.generated.h"

UCLASS()
class PROTO_BOAT_API ASLevelLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLevelLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SLevelLight")
	void OnLightTurnOn();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SLevelLight")
	void OnLightTurnOff();


	void TurnOn(bool TurnOn);

};
