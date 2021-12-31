// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAISightComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTO_BOAT_API USAISightComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAISightComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	float AlertSpeed = 0.2f;
	
	UPROPERTY(EditAnywhere)
	float Radius = 1000.0f;
	
	UPROPERTY(EditAnywhere)
	float Angle = 60.0f;
};
