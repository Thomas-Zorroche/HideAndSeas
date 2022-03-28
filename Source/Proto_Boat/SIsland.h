// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SIsland.generated.h"

UCLASS()
class PROTO_BOAT_API ASIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASIsland();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ASIsland")
	void OnIDReady();

	void SetID(uint8 ID) { IslandID = ID; }

	UFUNCTION(BlueprintCallable)
	uint8 GetID() const { return IslandID; }

	UPROPERTY(BlueprintReadWrite)
	FTransform ExitTransform;

private:
	// ID of Island inside GameManager.Islands
	uint8 IslandID;
};
