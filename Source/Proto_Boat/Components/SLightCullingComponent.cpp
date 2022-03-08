// Fill out your copyright notice in the Description page of Project Settings.


#include "SLightCullingComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/Character.h"
#include "../STopDownCharacter.h"



USLightCullingComponent::USLightCullingComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &USLightCullingComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USLightCullingComponent::OnEndOverlap);
}

void USLightCullingComponent::BeginPlay()
{
	Super::BeginPlay();

	auto Components = GetOwner()->GetComponentsByClass(ULightComponent::StaticClass());

	for (auto Component : Components)
	{
		auto LightComp = Cast<ULightComponent>(Component);
		if (LightComp)
		{
			OwnerLights.Add(LightComp);
		}
	}

	// If player already inside, toggle visibility
	auto PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	float PlayerToLightDistanceSqr = FVector::DistSquared(PlayerCharacter->GetActorLocation(), GetComponentLocation());
	if (PlayerToLightDistanceSqr <= SphereRadius * SphereRadius)
	{
		for (auto Light : OwnerLights)
		{
			Light->SetVisibility(true);
		}
	}
}

void USLightCullingComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ASTopDownCharacter>(OtherActor))
	{
		for (auto Light : OwnerLights)
		{
			Light->SetVisibility(true);
		}

	}
}

void USLightCullingComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ASTopDownCharacter>(OtherActor))
	{
		for (auto Light : OwnerLights)
		{
			Light->SetVisibility(false);
		}
	}
}


