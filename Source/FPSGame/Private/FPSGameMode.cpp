// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameState.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess) {
	if (!bIsGameOver) {
		if (InstigatorPawn) {

			bIsGameOver = true;

			if (!ensure(SpectatingViewpointClass != nullptr)) return;
			TArray<AActor*> ReturnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);

			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
				APlayerController* PC = It->Get();
				if (PC && ReturnedActors.Num() > 0) {
					PC->SetViewTargetWithBlend(ReturnedActors[0], 1.0f, EViewTargetBlendFunction::VTBlend_Cubic);
				}
			}
		}

		AFPSGameState* GS = GetGameState<AFPSGameState>();
		if (GS) {
			UE_LOG(LogTemp, Warning, TEXT("Log Message"));
			GS->MulticastOnMissionComplete(InstigatorPawn, bMissionSuccess);
		}

		OnMissionCompleted(InstigatorPawn, bMissionSuccess);
	}
}