// Fill out your copyright notice in the Description page of Project Settings.


#include "Bunker.h"

#include "Components/InstancedStaticMeshComponent.h"


// Sets default values
ABunker::ABunker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	InstancedMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ABunker::BeginPlay()
{
	Super::BeginPlay();

}

void ABunker::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BuildBunkerMesh();
}

void ABunker::ReceiveHit(int32 Item)
{
	if (Item < 0) return;

	InstancedMesh->RemoveInstance(Item);	
}

// Called every frame
void ABunker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABunker::BuildBunkerMesh()
{
	check(InstancedMesh);
	
	TArray<FTransform> BunkerBlockTransforms;

	// Right side
	const float BlockSize = 100.f;
	const float Offset = 300.f;
	for (int32 i = 0; i < 5; ++i)
	{
		for (int32 j = 0; j < 5; ++j)
		{
			BunkerBlockTransforms.Add(FTransform(FVector((BlockSize * j) + Offset, -i * BlockSize, 0.f)));
		}
	}

	// Left side
	for (int32 i = 0; i < 5; ++i)
	{
		for (int32 j = 0; j < 5; ++j)
		{
			BunkerBlockTransforms.Add(FTransform(FVector((-BlockSize * j) - Offset, -i * BlockSize, 0.f)));
		}
	}

	// Top side
	for (int32 i = 5; i < 8; ++i)
	{
		for (int32 j = 0; j < 15; ++j)
		{
			BunkerBlockTransforms.Add(FTransform(FVector((BlockSize * j) - BlockSize * 7, -BlockSize * i, 0.f)));
		}
	}
	
	InstancedMesh->ClearInstances();
	InstancedMesh->AddInstances(BunkerBlockTransforms, true);
}

