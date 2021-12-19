// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProceduralLevel.generated.h"


UENUM(BlueprintType)
enum class RoomType : uint8
{
	START = 0			UMETA(DisplayName = "Start"),
	END = 1				UMETA(DisplayName = "End"),
	LEFTTOBACK = 2		UMETA(DisplayName = "Left to Back"),
	FRONTTORIGHT = 3	UMETA(DisplayName = "Front to Right"),
	RIGHTTOLEFT = 4		UMETA(DisplayName = "Right to Left"),
	BACKTOFRONT	= 5		UMETA(DisplayName = "Back to Front"),
};

USTRUCT()
struct FRooms
{
	GENERATED_BODY()

	TArray< FString > RoomsName;
};



UCLASS()
class PROTO_BOAT_API ASProceduralLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProceduralLevel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	TMap< RoomType, FRooms > Rooms;
	UWorld* ProceduralWorld;

	UPROPERTY(EditAnywhere)
	float OffsetBetweenRoom;

};
