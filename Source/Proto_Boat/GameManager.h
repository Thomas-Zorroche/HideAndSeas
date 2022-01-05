// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "./ProceduralLevels/SRoomTemplate.h"
#include "./ProceduralLevels/SIslandLevel.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

UCLASS()
class PROTO_BOAT_API AGameManager : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TArray<USRoomTemplate*> PoolOfRoom;
	TArray<USIslandLevel*> Islands;
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
