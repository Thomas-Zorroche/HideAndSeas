// Fill out your copyright notice in the Description page of Project Settings.


#include "SPatrollerController.h"


FRotator ASPatrollerController::GetControlRotation() const
{
    if (GetPawn() == nullptr)
    {
        return FRotator();
    }

    return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}
