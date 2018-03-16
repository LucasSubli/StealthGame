// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAiGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"
#include "AI/Navigation/NavigationSystem.h"


// Sets default values
AFPSAiGuard::AFPSAiGuard() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComp"));
}

// Called when the game starts or when spawned
void AFPSAiGuard::BeginPlay() {
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();

	SensingComp->OnSeePawn.AddDynamic(this, &AFPSAiGuard::OnPawnSeen);
	SensingComp->OnHearNoise.AddDynamic(this, &AFPSAiGuard::OnNoiseHeard);
	
	if (bPatrol) {
		MoveToNextPatrolPoint();
	}
}

void AFPSAiGuard::OnPawnSeen(APawn* SeenPawn) {

	SetGuardState(EAIState::Alerted);

	if (!ensure(SeenPawn != nullptr)) return;
	//DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->CompleteMission(SeenPawn, false);
	}

	// Stop Movement if Patrolling
	AController* Controller = GetController();
	if (Controller) {
		Controller->StopMovement();
	}
}

void AFPSAiGuard::OnNoiseHeard(APawn* NoiseMaker, const FVector& Location, float Volume) {

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);
	if (GuardState == EAIState::Alerted) return;

	SetGuardState(EAIState::Suspicious);
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = .0f;
	NewLookAt.Roll = .0f;
	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAiGuard::ResetOrientation, 3.0f);

	AController* Controller = GetController();
	if (Controller) {
		Controller->StopMovement();
	}
}

void AFPSAiGuard::ResetOrientation() {
	if (GuardState == EAIState::Alerted) return;
	SetGuardState(EAIState::Idle);
	SetActorRotation(OriginalRotation);

	// Stopped investigating...if we are a patrolling pawn, pick a new patrol point to move to
	if (bPatrol) {
		MoveToNextPatrolPoint();
	}
}

void AFPSAiGuard::SetGuardState(EAIState NewState) {
	if (GuardState == NewState) return;
	
	GuardState = NewState;
	OnRep_GuardState();
}

void AFPSAiGuard::MoveToNextPatrolPoint() {
	// Assign next patrol point.
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint) {
		CurrentPatrolPoint = FirstPatrolPoint;
	} else {
		CurrentPatrolPoint = SecondPatrolPoint;
	}

	UNavigationSystem::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

void AFPSAiGuard::OnRep_GuardState() {
	OnStateChanged(GuardState);
}

// Called every frame
void AFPSAiGuard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Patrol Goal Checks
	if (CurrentPatrolPoint) {
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		// Check if we are within 50 units of our goal, if so - pick a new patrol point
		if (DistanceToGoal < 50) {
			MoveToNextPatrolPoint();
		}
	}
}

void AFPSAiGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAiGuard, GuardState);
}

