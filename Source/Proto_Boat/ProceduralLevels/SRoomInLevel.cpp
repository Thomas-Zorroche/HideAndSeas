// Fill out your copyright notice in the Description page of Project Settings.


#include "./SRoomInLevel.h"

void USRoomInLevel::Init(int poolIndex, FVector position, bool isFinished) {
	PoolIndex = poolIndex;
	WorldPosition = position;
	IsFinished = isFinished;
}