// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h" 

#include "SEmptyMarker.generated.h"

UCLASS()
class PROTO_BOAT_API ASEmptyMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASEmptyMarker();

	void SetColor(FColor Color);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetIndex(int index) { Index = index; }

private:
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere)
	int Index = 0;
};
