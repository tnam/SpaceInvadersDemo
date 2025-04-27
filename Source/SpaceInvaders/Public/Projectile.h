// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/PooledObjectInterface.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class SPACEINVADERS_API AProjectile : public AActor, public IPooledObjectInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
