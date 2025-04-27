// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Projectile.h"
#include "Interfaces/SIDestructibleInterface.h"

#include "Ship.generated.h"

class UNiagaraSystem;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class UFloatingPawnMovement;

UCLASS()
class SPACEINVADERS_API AShip : public APawn, public ISIDestructibleInterface
{
	GENERATED_BODY()
	
protected:
	
	// Enhanced Input Mapping Context 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// Move Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	// Fire Input Action
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float FireRate = 0.2f;

	FTimerHandle FireTimerHandle;
	bool bCanFire = true;

private:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> CollisionBox;

	// Static mesh used by the ship 
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MainMesh;
	
	// Movement component used to move the ship
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UFloatingPawnMovement> ShipMovementComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ExplosionFX;
	
public:
	// Sets default values for this pawn's properties
	AShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called for movement input
	void Move(const FInputActionValue& Value);
	
	// Called for fire input
	void Fire(const FInputActionValue& Value);

	// Ship can fire again  
	void EnableFire();

	// Player death FX
	void SpawnExplosionFX();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void ReceiveHit(int32 Item = -1) override;
};
