// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAiGuard.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8 {
	Idle,
	Suspicious,
	Alerted
};

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

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

	void MoveToNextPatrolPoint();

	// Sensor component for seeing and hearing
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* SensingComp;

	// Original rotation
	FRotator OriginalRotation;

	// Timer for distraction
	FTimerHandle TimerHandle_ResetOrientation;

	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
	EAIState GuardState = EAIState::Idle;

	/** Not called on the server **/
	UFUNCTION()
	void OnRep_GuardState();

	/* Let the guard go on patrol */
	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol = false;

	/* First of two patrol points to patrol between */
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	AActor* FirstPatrolPoint;

	/* Second of two patrol points to patrol between */
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	AActor* SecondPatrolPoint;

	// The current point the actor is either moving to or standing at
	AActor* CurrentPatrolPoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
