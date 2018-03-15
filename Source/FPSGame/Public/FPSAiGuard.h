// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAiGuard.generated.h"

UCLASS()
class FPSGAME_API AFPSAiGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAiGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when some pawn is seen
	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	// Called when something is heard
	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseMaker, const FVector& Location, float Volume);

	// Called when something is heard
	UFUNCTION()
	void ResetOrientation();

	// Sensor component for seeing and hearing
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* SensingComp;

	// Original rotation
	FRotator OriginalRotation;

	// Timer for distraction
	FTimerHandle TimerHandle_ResetOrientation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
