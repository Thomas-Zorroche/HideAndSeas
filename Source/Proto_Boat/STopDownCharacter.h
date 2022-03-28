// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STopDownCharacter.generated.h"

UCLASS()
class PROTO_BOAT_API ASTopDownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTopDownCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isVisible = true;

	UPROPERTY(BlueprintReadWrite)
	bool DebugMode = false;

	UPROPERTY(BlueprintReadWrite)
	bool InsideTransition = false;

};
