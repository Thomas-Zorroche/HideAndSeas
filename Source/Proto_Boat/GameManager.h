// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Managers/SLevelManager.h"
#include "GameManager.generated.h"


UCLASS()
class PROTO_BOAT_API UGameManager : public UGameInstance
{
	GENERATED_BODY()

public:
	void Init() override;

	void Shutdown() override;

	// Return Exit Island Position. Needed when player leave an island.
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	FTransform GetBoatSpawnPosition(TArray<ASIsland*> Islands);
	
	UFUNCTION(BlueprintCallable, Category = "GameManager")
	int GetMaximumMana() const;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameManager")
	USLevelManager* LevelManager = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ChampiClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> GolemClass;

	UPROPERTY(BlueprintReadOnly)
	uint8 WizardCrystalID;

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> CrystalsInBag;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor CurrentCrystalColor;


};

