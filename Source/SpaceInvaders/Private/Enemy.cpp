// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EnemiesController.h"
#include "NiagaraFunctionLibrary.h"
#include "PoolSubsystem.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"

#include "SICollisionChannels.h"
#include "SIGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(SI_ObjectChannel_Enemy);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionBox);
}

void AEnemy::SetEnemyData(FEnemyData* Data)
{
	if (Data == nullptr) return;

	Mesh->SetStaticMesh(Data->Mesh.LoadSynchronous());
	Points = Data->Points;
}

void AEnemy::FireProjectile()
{
	// Spawn projectile from pool
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{
		FPoolSpawnParameters SpawnParams;
		SpawnParams.ActorClass = ProjectileClass;
		SpawnParams.SpawnTransform = FTransform(FRotator::ZeroRotator, GetActorLocation());
		SpawnParams.Owner = this;
		SpawnParams.Lifetime = 5.f;
		PoolSubsystem->SpawnActorFromPool<AProjectile>(SpawnParams);
	}
}

void AEnemy::PooledActorDespawned()
{
	check(EnemiesController.IsValid());

	EnemiesController->OnEnemyHit(this);
}

void AEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (GetOwner() == OtherActor || OtherActor == nullptr || OtherComponent == nullptr) return;
	
	const ECollisionChannel ObjectType = OtherComponent->GetCollisionObjectType();

	// End game when colliding with the player
	if (ObjectType == ECC_Pawn)
	{
		if (ASIGameMode* GM = Cast<ASIGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->EndGame();
			return;
		}
	}

	if (ISIDestructibleInterface* DestructibleObject = Cast<ISIDestructibleInterface>(OtherActor))
	{
		DestructibleObject->ReceiveHit(Hit.Item);
	}
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemiesController = Cast<AEnemiesController>(GetOwner());
	if (EnemiesController.IsValid())
	{
		ProjectileClass = EnemiesController->ProjectileClass;		
	}
	
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnBeginOverlap);
}

void AEnemy::ReceiveHit(int32 Item)
{
	SpawnExplosionFX();
	
	if (ASIGameMode* GM = Cast<ASIGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->AddKillScore(Points);	
	}
	
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{
		PoolSubsystem->ReturnActorToPool(this);
	}
}

void AEnemy::SpawnExplosionFX()
{
	if (!ExplosionFX) nullptr;
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation());	
}


