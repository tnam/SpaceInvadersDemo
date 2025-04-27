// Fill out your copyright notice in the Description page of Project Settings.


#include "SIGameMode.h"

#include "Bunker.h"
#include "EnemiesController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ASIGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);		
	Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	EnemiesController = Cast<AEnemiesController>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemiesController::StaticClass()));
	PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));

	TArray<AActor*> FoundBunkers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABunker::StaticClass(), FoundBunkers);

	for(const auto& Actor : FoundBunkers)
	{
		if (ABunker* Bunker = Cast<ABunker>(Actor))
		{
			Bunkers.Add(Bunker);
		}
	}

	CreatePlayerHud();
	OnPlayerLivesUpdated.Broadcast(PlayerLives);
	OnPlayerScoreUpdated.Broadcast(0);
}

void ASIGameMode::OnPlayerHit()
{
	if (PlayerLives > 0)
	{
		PlayerLives--;

		OnPlayerLivesUpdated.Broadcast(PlayerLives);

		if (PlayerLives > 0)
		{
			RestartGame();
		}
		else
		{
			EndGame();
		}
	}
}

void ASIGameMode::EndGame()
{
	if (Pawn.IsValid())
	{
		Pawn->Destroy();
	}

	if (EnemiesController.IsValid())
	{
		EnemiesController->StopEnemies();
		EnemiesController->DeactivateEnemyProjectiles();
	}

	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);

		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}
	
	if (PlayerController.IsValid())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

void ASIGameMode::RestartGame()
{
	if (EnemiesController.IsValid())
	{
		EnemiesController->StopEnemies();
		EnemiesController->DeactivateEnemyProjectiles();
	}
	
	RespawnPlayer();
	SetPawnInputEnabled(false);

	FTimerHandle RestartTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RestartTimerHandle, [this]()
	{
		if (EnemiesController.IsValid())
		{
			SetPawnInputEnabled(true);
			EnemiesController->RestartEnemies();
		}
	}, RestartTime, false);  
	
}

void ASIGameMode::StartNewEnemyWave()
{
	if (!EnemiesController.IsValid()) return;

	EnemiesController->RespawnEnemies();

	for (const auto& Bunker : Bunkers)
	{
		Bunker->BuildBunkerMesh();
	}
}

void ASIGameMode::AddKillScore(int32 Score)
{
	PlayerScore += Score;
	OnPlayerScoreUpdated.Broadcast(PlayerScore);
}

void ASIGameMode::CreatePlayerHud()
{
	if (PlayerHudWidgetClass)
	{
		PlayerHudWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHudWidgetClass);

		if (PlayerHudWidget)
		{
			PlayerHudWidget->AddToViewport();
		}
	}
}

void ASIGameMode::RespawnPlayer()
{
	APawn* ExistingPawn = FindPawn(); 
	if (ExistingPawn == nullptr) return;

	ExistingPawn->Destroy();	
	
	Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, FindPlayerStartLocation(), FRotator::ZeroRotator);
    if (Pawn.IsValid())
    {
        PlayerController->Possess(Pawn.Get());
    }
}

void ASIGameMode::SetPawnInputEnabled(bool bEnable)
{
	APawn* ExistingPawn = FindPawn();
	APlayerController* PC = FindPlayerController();
	if (!ExistingPawn || ! PC) return;

	if (bEnable)
	{
		ExistingPawn->EnableInput(PC);		
	}
	else
	{
		ExistingPawn->DisableInput(PC);	
	}
}

APawn* ASIGameMode::FindPawn() 
{
	if (Pawn == nullptr)
	{
		Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	return Pawn.Get();
}

APlayerController* ASIGameMode::FindPlayerController()
{
	if (PlayerController == nullptr)
	{
		PlayerController = UGameplayStatics::GetPlayerController(this, 0);		
	}

	return PlayerController.Get();
}

FVector ASIGameMode::FindPlayerStartLocation()
{
	FVector StartLocation;
	
	if (PlayerStart == nullptr)
	{
		PlayerStart = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
	}

	if (PlayerStart.IsValid())
	{
		StartLocation = PlayerStart->GetActorLocation();
	}

	return StartLocation;
}

