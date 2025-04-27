// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolSubsystem.h"

#include "Interfaces/PooledObjectInterface.h"


void UPoolSubsystem::ReturnActorToPool(AActor* PooledActor)
{
	if (PooledActor == nullptr) return;

	if (IPooledObjectInterface* PooledObjectInstance = Cast<IPooledObjectInterface>(PooledActor))
	{
		SetActorActive(PooledActor, false);
		PooledObjectInstance->PooledActorDespawned();
	}
	else
	{
		PooledActor->Destroy();
	}
}

void UPoolSubsystem::SetActorActive(AActor* PooledActor, bool bActive, float Lifetime /* = 0.f */)
{
	if (PooledActor == nullptr) return;

	const UClass* PooledActorClass = PooledActor->GetClass();
	FPoolData* ObjectPool = ObjectPoolMap.Find(PooledActorClass);
	if (ObjectPool == nullptr) return;
	
	if (bActive)
	{
		// Return if already active
		if (ObjectPool->ActiveObjects.Contains(PooledActor)) return;
	
		ObjectPool->ActiveObjects.AddUnique(PooledActor);

		if (ObjectPool->InactiveObjects.Contains(PooledActor))
		{
			ObjectPool->InactiveObjects.Remove(PooledActor);
		}

		if (Lifetime > 0.f)
		{
			FTimerHandle& TimerHandle = ObjectPool->GetTimerHandleForActor(PooledActor);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PooledActor]()
			{
				DeactivateActor(PooledActor);		
			},
			Lifetime,
			false);
		}
	}
	else 
	{
		if (ObjectPool->InactiveObjects.Contains(PooledActor)) return;
			
		ObjectPool->InactiveObjects.AddUnique(PooledActor);
		
		if (ObjectPool->ActiveObjects.Contains(PooledActor))
		{
			ObjectPool->ActiveObjects.Remove(PooledActor);
		}

		if (ObjectPool->ActiveTimerHandles.Contains(PooledActor))
		{
			FTimerHandle TimerHandle = ObjectPool->ActiveTimerHandles[PooledActor];
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			ObjectPool->ActiveTimerHandles.Remove(PooledActor);
		}
	}

	PooledActor->SetActorHiddenInGame(!bActive);
	PooledActor->SetActorEnableCollision(bActive);
	PooledActor->SetActorTickEnabled(bActive);
}

void UPoolSubsystem::DeactivateActor(AActor* Actor)
{
	SetActorActive(Actor, false);
}

void UPoolSubsystem::DeactivateAllActorsOfClass(UClass* ActorClassToDeactivate)
{
	FPoolData* ObjectPool = ObjectPoolMap.Find(ActorClassToDeactivate);
	if (ObjectPool == nullptr) return;

	for (const auto& ActiveObject : ObjectPool->ActiveObjects)
	{
		ActiveObject->SetActorHiddenInGame(true);
		ActiveObject->SetActorEnableCollision(false);
		ActiveObject->SetActorTickEnabled(false);
		ObjectPool->InactiveObjects.AddUnique(ActiveObject);
	}

	for (const auto& ActiveTimer : ObjectPool->ActiveTimerHandles)
	{
		FTimerHandle TimerHandle = ActiveTimer.Value;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	ObjectPool->ActiveTimerHandles.Empty();
	ObjectPool->ActiveObjects.Empty();
}
