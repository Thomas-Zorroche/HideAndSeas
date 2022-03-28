// Fill out your copyright notice in the Description page of Project Settings.


#include "./GameManager.h"
#include "./SIsland.h"


void UGameManager::Init() 
{
	Super::Init();

	LevelManager = NewObject<USLevelManager>(this, USLevelManager::StaticClass());
	LevelManager->Initialize();

	WizardCrystalID = FMath::FRandRange(0, 7);
	CurrentCrystalColor = { 1., 1., 1., 1. };
}

void UGameManager::Shutdown()
{
	Super::Shutdown();
}


FTransform UGameManager::GetBoatSpawnPosition(TArray<ASIsland*> ActorIslands) 
{
	for (auto Island : ActorIslands) {
		if (Island->GetID() == LevelManager->CurrentIslandID) {
			return Island->ExitTransform;
		}
	}
	return FTransform();
}

int UGameManager::GetMaximumMana() const
{
	FLinearColor FirstColor = { 1., 1., 1., 1. };

	if (CurrentCrystalColor == LevelManager->CrystalColors[WizardCrystalID])
		return 150;

	if (CurrentCrystalColor == FirstColor)
		return 30;

	return 100;
}

int UGameManager::GetMaximumManaFromColor(const FLinearColor& Color) const
{
	FLinearColor FirstColor = { 1., 1., 1., 1. };

	if (Color == LevelManager->CrystalColors[WizardCrystalID])
		return 150;

	if (Color == FirstColor)
		return 30;

	return 100;
}


