// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SIDestructibleInterface.h"
#include "Bunker.generated.h"

UCLASS()
class SPACEINVADERS_API ABunker : public AActor, public ISIDestructibleInterface
{
private:
	GENERATED_BODY()

private:
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Root;
	
	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedMesh;

public:
	// Sets default values for this actor's properties
	ABunker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void ReceiveHit(int32 Item = -1) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Constructs the bunker mesh
	void BuildBunkerMesh();
};
