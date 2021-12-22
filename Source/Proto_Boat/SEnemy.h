// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Containers/UnrealString.h" 
#include "SEnemy.generated.h"

UCLASS()
class PROTO_BOAT_API ASEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateLightLevel(float AlertLevel);
	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemy")
	void OnLightLevelChanged(const float currentLightIntensity);

	void UpdateDebugStateLabel(const FString& Label);
	UFUNCTION(BlueprintImplementableEvent, Category = "SEnemy")
	void OnDebugStateLabelChanged(const FString& debugStateLabel);

protected:
	UPROPERTY(EditAnywhere)
	float BaseLightIntensity = 500.0f;
	UPROPERTY(EditAnywhere)
	float MaxLightIntensity = 5000.0f;

private:
	float CurrentLightIntensity = BaseLightIntensity;
	// 0: base intensity ; 1: max intensity 
	float LightLevel = 0.0f;
	FString DebugStateLabel = "PATROL";
};
