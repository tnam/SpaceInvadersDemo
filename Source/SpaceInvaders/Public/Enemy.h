// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Projectile.h"
#include "Interfaces/SIDestructibleInterface.h"

#include "Enemy.generated.h"


class UNiagaraSystem;
class AEnemiesController;
class UBoxComponent;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase
{
    GENERATED_BODY()

public:
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ID;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Points;
};

UCLASS()
class SPACEINVADERS_API AEnemy : public AActor, public IPooledObjectInterface, public ISIDestructibleInterface
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	
	// Static mesh used by the enemy 
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ExplosionFX;

	TSubclassOf<AProjectile> ProjectileClass;

	TWeakObjectPtr<AEnemiesController> EnemiesController;

	// Points assigned on enemy kill
	int32 Points;

public:
	// Sets default values for this actor's properties
	AEnemy();

	// Initialize enemy 
	void SetEnemyData(FEnemyData* Data);
	
	void FireProjectile();

	// IPooledObjectInterface overrides
	void PooledActorDespawned() override;

protected:
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void ReceiveHit(int32 Item = -1) override;

	void SpawnExplosionFX();
};
