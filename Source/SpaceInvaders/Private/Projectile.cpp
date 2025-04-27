// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "PoolSubsystem.h"
#include "SICollisionChannels.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/SIDestructibleInterface.h"


// Sets default values
AProjectile::AProjectile()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToChannel(SI_ObjectChannel_EnemyGroup, ECR_Ignore);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	if (Mesh)
	{
		Mesh->SetupAttachment(CollisionBox);
	}
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = 500.f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionBox)
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
	}
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (GetOwner() == OtherActor || OtherActor == nullptr || OtherComponent == nullptr) return;

	const ECollisionChannel ObjectType = OtherComponent->GetCollisionObjectType();

	if (ISIDestructibleInterface* DestructibleObject = Cast<ISIDestructibleInterface>(OtherActor))
	{
		DestructibleObject->ReceiveHit(Hit.Item);
	}
	
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{
		// Return projectile to the pool
		PoolSubsystem->ReturnActorToPool(this);
	}
	else
	{
		Destroy();
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

