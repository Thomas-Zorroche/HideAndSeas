// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IsEnemy.h"
#include "SPatroller.generated.h"

UCLASS()
class PROTO_BOAT_API ASPatroller : public ACharacter, public IIsEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASPatroller();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual class USAISightComp* GetAISightComp() override;

public:
	UPROPERTY(VisibleAnywhere)
	class USAISightComp* AISightComp;
};
