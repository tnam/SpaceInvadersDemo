// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "GameFramework/Actor.h"
#include "EnemiesController.generated.h"

UCLASS()
class SPACEINVADERS_API AEnemiesController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemiesController();

	// Number of enemy rows
	UPROPERTY(EditDefaultsOnly)
	int32 Rows = 5;
	
	// Number of enemy columns
	UPROPERTY(EditDefaultsOnly)
	int32 Columns = 5;
	
	// Spacing between enemies 
	UPROPERTY(EditDefaultsOnly)
	float Spacing = 100.f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemy> EnemyToSpawn;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float FireRate = 0.8f;
	
private:
	
	UPROPERTY()
	USceneComponent* Scene;
	
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* CollisionBox;
	
	UPROPERTY(EditDefaultsOnly, meta = (RequiredAssetDataTags = "RowStructure=/Script/SpaceInvaders.EnemyData"))
	UDataTable* EnemyDataTable = nullptr;

	UPROPERTY()
	TArray<AActor*> SpawnedEnemies;

	FVector MoveDirection;

	// Moving speed of enemies
	float MovingSpeed = 1.f;
	
	// How much MovingSpeed is increased every time an enemy is destroyed 
	float SpeedMultiplier = 1.05f;

	// Initial spawn location of enemies 
	FVector StartLocation;

	FTimerHandle MoveTimerHandle;
	FTimerHandle AttackTimerHandle;

	TMap<int32, FEnemyData*> EnemyDataMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Initialize this actor using a data table
	void InitEnemyData();

	// Moves the enemy swarm 
	void MoveEnemies();

	// Picks a random enemy to attack
	void PerformRandomAttack();

	// Updates the bounding box used for boundaries check 
	void UpdateEnemiesCollision();

	UFUNCTION()
	void OnEnemiesOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

public:

	void SpawnEnemyWave();
	
	void OnEnemyHit(AEnemy* HitEnemy);

	void StopEnemies();

	void RestartEnemies();
	
	void DeactivateEnemyProjectiles();
	
	void RespawnEnemies();
};
