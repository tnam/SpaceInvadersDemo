// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PooledObjectInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubsystem.generated.h"

struct SPACEINVADERS_API FPoolSpawnParameters
{
public:
	TSubclassOf<AActor> ActorClass;
	FTransform SpawnTransform;

	AActor* Owner;
	APawn* Instigator;

	float Lifetime;

	FPoolSpawnParameters()
		: ActorClass(nullptr)
		, SpawnTransform(FTransform::Identity)
		, Owner(nullptr)
		, Instigator(nullptr)
		, Lifetime(0.f)
	{}
};

USTRUCT()
struct FPoolData 
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ActiveObjects;
	
	UPROPERTY()
	TArray<AActor*> InactiveObjects;

	UPROPERTY()
	TMap<AActor*, FTimerHandle> ActiveTimerHandles; 

	bool HasInactiveObjects() const
	{
		return !InactiveObjects.IsEmpty();
	}

	AActor* Top()
	{
		return InactiveObjects.Top();
	}

	FTimerHandle& GetTimerHandleForActor(AActor* Actor)
	{
		return ActiveTimerHandles.FindOrAdd(Actor);	
	}
};

class IPooledObjectInterface;

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API UPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// Spawns a pooled actor and sets it as active   
	template<typename PooledObjectClass>
	PooledObjectClass* SpawnActorFromPool(const FPoolSpawnParameters& SpawnParams);

	// Deactivates an actor and returns it to the pool
	void ReturnActorToPool(AActor* PooledActor);

	// Deactivates all pooled actors of the specified class 
	void DeactivateAllActorsOfClass(UClass* ActorClassToDeactivate);

private:

	// Updates visibility, collision, tick of a pooled actor
	void SetActorActive(AActor* Actor, bool bActive, float LifeTime = 0.f);

	// Deactivates a pooled actor
	UFUNCTION()
	void DeactivateActor(AActor* Actor);

	// Internal object pool map 
	UPROPERTY()
	TMap<UClass*, FPoolData> ObjectPoolMap;
};

template <typename PooledObjectClass>
PooledObjectClass* UPoolSubsystem::SpawnActorFromPool(const FPoolSpawnParameters& SpawnParams)
{
	PooledObjectClass* PooledActor = nullptr;
	
	if (SpawnParams.ActorClass->ImplementsInterface(UPooledObjectInterface::StaticClass()))
	{
		FPoolData& ObjectPool = ObjectPoolMap.FindOrAdd(SpawnParams.ActorClass);
		
		if (!ObjectPool.HasInactiveObjects())
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ActorSpawnParams.Owner = SpawnParams.Owner;
			ActorSpawnParams.Instigator = SpawnParams.Instigator;
			PooledActor = GetWorld()->SpawnActor<PooledObjectClass>(SpawnParams.ActorClass, SpawnParams.SpawnTransform, ActorSpawnParams);
		}
		else
		{
			PooledActor = CastChecked<PooledObjectClass>(ObjectPool.Top());
			if (PooledActor)
			{
				PooledActor->SetActorTransform(SpawnParams.SpawnTransform);
			}
		}

		SetActorActive(PooledActor, true, SpawnParams.Lifetime);
	}

	if (IPooledObjectInterface* PooledObjectInstance = Cast<IPooledObjectInterface>(PooledActor))
	{
		PooledObjectInstance->PooledActorSpawned();	
	}

	return PooledActor;
}
