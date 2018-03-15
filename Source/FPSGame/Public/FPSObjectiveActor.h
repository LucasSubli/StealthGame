// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSObjectiveActor.generated.h"

UCLASS()
class FPSGAME_API AFPSObjectiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSObjectiveActor();

protected:

	// Mesh for the objective
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent * MeshComp;

	// Collision presets
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComponent;

	// Collision presets
	UPROPERTY(EditDefaultsOnly, Category = "Fx")
	class UParticleSystem* PickupFx;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Play Sound and particle effects
	void PlayEffects();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	
	
};
