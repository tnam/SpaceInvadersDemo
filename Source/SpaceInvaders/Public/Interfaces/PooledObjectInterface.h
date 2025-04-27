// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PooledObjectInterface.generated.h"

UINTERFACE()
class UPooledObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPACEINVADERS_API IPooledObjectInterface
{
	GENERATED_BODY()

public:

	virtual void PooledActorSpawned() {};

	virtual void PooledActorDespawned() {}; 
};
