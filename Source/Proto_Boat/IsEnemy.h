// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IsEnemy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIsEnemy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROTO_BOAT_API IIsEnemy
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class USAISightComp* GetAISightComp();
};
