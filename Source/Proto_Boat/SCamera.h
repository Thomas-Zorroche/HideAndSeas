// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IsEnemy.h"
#include "SCamera.generated.h"

UCLASS()
class PROTO_BOAT_API ASCamera : public APawn, public IIsEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual class USEnemyComponent* GetEnemyComp() override;

	virtual bool IsInsidePlayerTile() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SCamera")
	void OnRoomComplete();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "SCamera")
	void OnPlayerTileChanged();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USEnemyComponent* EnemyComp;

	UPROPERTY(BlueprintReadOnly)
	bool InsidePlayerTile = false;

	UPROPERTY(BlueprintReadOnly)
	bool IsAlive = true;
};
