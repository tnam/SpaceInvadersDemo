// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SIGameMode.generated.h"

class ABunker;
class APlayerStart;
class AEnemiesController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatUpdated, int32, NewValue);

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API ASIGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatUpdated OnPlayerLivesUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatUpdated OnPlayerScoreUpdated;

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	int32 PlayerLives = 3;
	
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	int32 PlayerScore = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	float RestartTime = 2.f;

public:

	virtual void BeginPlay() override;

	void OnPlayerHit();

	void EndGame();

	void RestartGame();

	void StartNewEnemyWave();

	void AddKillScore(int32 Score);

protected:

	void CreatePlayerHud();
	
	void RespawnPlayer();
	
	void SetPawnInputEnabled(bool bEnable);

	APawn* FindPawn();

	APlayerController* FindPlayerController();

	FVector FindPlayerStartLocation();

private:

	// Widget class to spawn
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
    
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHudWidgetClass;

    // Pointer to hold the created widget instance
    UPROPERTY()
    UUserWidget* GameOverWidget;
    
    UPROPERTY()
    UUserWidget* PlayerHudWidget;

	UPROPERTY()
	TArray<ABunker*> Bunkers;

	TWeakObjectPtr<APlayerController> PlayerController;
	TWeakObjectPtr<APawn> Pawn;
	TWeakObjectPtr<AEnemiesController> EnemiesController;
	TWeakObjectPtr<APlayerStart> PlayerStart;

};
