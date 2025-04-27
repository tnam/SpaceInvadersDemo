// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SIDestructibleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USIDestructibleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPACEINVADERS_API ISIDestructibleInterface
{
	GENERATED_BODY()

public:
	
	virtual void ReceiveHit(int32 Item = -1) {};
};
