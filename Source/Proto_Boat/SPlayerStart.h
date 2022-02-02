// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SLevelLoaded.h"
#include "SPlayerStart.generated.h"

UCLASS()
class PROTO_BOAT_API ASPlayerStart : public AActor, public ISLevelLoaded
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPlayerStart();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Broadcast")
	void OnLevelLoaded();  // This is the prototype declared in the interface
	virtual void OnLevelLoaded_Implementation() override; // This is the declaration of the implementation

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SPlayerStart")
	void OnPlayerStart();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
