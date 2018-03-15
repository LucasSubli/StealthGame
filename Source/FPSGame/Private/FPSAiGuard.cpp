// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAiGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "FPSGameMode.h"


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
	
}

void AFPSAiGuard::OnPawnSeen(APawn* SeenPawn) {

	if (!ensure(SeenPawn != nullptr)) return;
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->CompleteMission(SeenPawn, false);
	}
}

void AFPSAiGuard::OnNoiseHeard(APawn* NoiseMaker, const FVector& Location, float Volume) {
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = .0f;
	NewLookAt.Roll = .0f;
	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAiGuard::ResetOrientation, 3.0f);
}

void AFPSAiGuard::ResetOrientation() {
	SetActorRotation(OriginalRotation);
}

// Called every frame
void AFPSAiGuard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

