// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemiesController.h"

#include "PoolSubsystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "SICollisionChannels.h"
#include "SIGameMode.h"

// Sets default values
AEnemiesController::AEnemiesController()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Scene);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(Scene);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(SI_ObjectChannel_EnemyGroup);
	CollisionBox->SetCollisionResponseToChannel(SI_ObjectChannel_PlayerProjectile, ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(SI_ObjectChannel_EnemyProjectile, ECR_Ignore);

	MoveDirection = GetActorRightVector();
}

// Called when the game starts or when spawned
void AEnemiesController::BeginPlay()
{
	Super::BeginPlay();

	InitEnemyData();

	StartLocation = GetActorLocation();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemiesController::OnEnemiesOverlap);

	SpawnEnemyWave();
	MoveEnemies();
	PerformRandomAttack();
}

void AEnemiesController::InitEnemyData()
{
	// Initialize using data table
	if (EnemyDataTable)
	{
		TArray<FEnemyData*> EnemyDataRows;
		EnemyDataTable->GetAllRows<FEnemyData>("ContextString", EnemyDataRows);
		for (FEnemyData* DataRow : EnemyDataRows)
		{
			EnemyDataMap.Add(DataRow->ID, DataRow);
		}
	}
}

void AEnemiesController::MoveEnemies()
{
	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, [this]()
	{
		const FVector MoveLocation = GetActorLocation() + MoveDirection * MovingSpeed;
		SetActorLocation(MoveLocation);
	}, 0.05f, true);  
}

void AEnemiesController::PerformRandomAttack()
{
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, [this]()
	{
		if (SpawnedEnemies.IsEmpty()) return;

		const int32 RandomEnemyIndex = FMath::RandRange(0, SpawnedEnemies.Num() - 1);
		AEnemy* SpawnedEnemy = Cast<AEnemy>(SpawnedEnemies[RandomEnemyIndex]);
		if (SpawnedEnemy)
		{
			SpawnedEnemy->FireProjectile();
		}
	}, FireRate, true);  
	
}

void AEnemiesController::UpdateEnemiesCollision()
{
	check(CollisionBox);
	
	FVector Center;
	FVector BoxExtent;
	UGameplayStatics::GetActorArrayBounds(SpawnedEnemies, false, Center, BoxExtent);

	CollisionBox->SetBoxExtent(BoxExtent);
	CollisionBox->SetWorldLocation(Center);
}

void AEnemiesController::RespawnEnemies()
{
	MovingSpeed = 1.f;
	
	SetActorLocation(StartLocation);
	SpawnEnemyWave();
}

void AEnemiesController::DeactivateEnemyProjectiles()
{
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{
		PoolSubsystem->DeactivateAllActorsOfClass(ProjectileClass);
	}
}

void AEnemiesController::OnEnemiesOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	const ECollisionChannel ObjectType = OtherComponent->GetCollisionObjectType();

	if (ObjectType == SI_ObjectChannel_Boundary)
	{
		SetActorLocation(GetActorLocation() - GetActorForwardVector() * Spacing);
		MoveDirection *= -1.f;
	}
}

void AEnemiesController::SpawnEnemyWave()
{
	if (EnemyToSpawn == nullptr || EnemyDataMap.IsEmpty()) return;

	TArray<FEnemyData*> EnemyDataArray;
	EnemyDataMap.GenerateValueArray(EnemyDataArray);
		
	const float TotalWidth = (Columns - 1) * Spacing;
	const float TotalHeight = (Rows - 1) * Spacing;

	for (int32 Row = 0; Row < Rows; ++Row)
	{
		// For now spawns a random enemy type for each row
		const int32 RandomEnemyIndex = FMath::RandRange(0, EnemyDataArray.Num() - 1);
		
		for (int32 Col = 0; Col < Columns; ++Col)
		{
			const FVector SpawnLocation = GetActorLocation() + FVector(-Row * Spacing, Col * Spacing, 0.f);

			AEnemy* SpawnedEnemy = nullptr;
			if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
			{
				FPoolSpawnParameters SpawnParams;
				SpawnParams.ActorClass = EnemyToSpawn;
				SpawnParams.SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnLocation);
				SpawnParams.Owner = this;
				SpawnedEnemy = PoolSubsystem->SpawnActorFromPool<AEnemy>(SpawnParams);
			}
			
			if (SpawnedEnemy)
			{
				SpawnedEnemy->SetEnemyData(EnemyDataArray[RandomEnemyIndex]);
				SpawnedEnemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
				SpawnedEnemies.Add(SpawnedEnemy);
			}
		}
	}

	UpdateEnemiesCollision();
}

void AEnemiesController::OnEnemyHit(AEnemy* HitEnemy)
{
	if (SpawnedEnemies.IsEmpty()) return;
	
	SpawnedEnemies.Remove(HitEnemy);

	// Start a new wave when all enemies are killed 
	if (SpawnedEnemies.IsEmpty())
	{
		if (ASIGameMode* GM = Cast<ASIGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->StartNewEnemyWave();	
		}
	}

	MovingSpeed *= SpeedMultiplier;
	UpdateEnemiesCollision();
}

void AEnemiesController::StopEnemies()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(MoveTimerHandle))
	{
		TimerManager.ClearTimer(MoveTimerHandle);	
	}
	
	if (TimerManager.IsTimerActive(AttackTimerHandle))
	{
		TimerManager.ClearTimer(AttackTimerHandle);	
	}
}

void AEnemiesController::RestartEnemies()
{
	MoveEnemies();
	PerformRandomAttack();
}

