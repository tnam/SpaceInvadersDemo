// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "PoolSubsystem.h"
#include "SICollisionChannels.h"
#include "SIGameMode.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShip::AShip()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(TEXT("Pawn"));
	CollisionBox->SetCollisionResponseToChannel(SI_ObjectChannel_PlayerProjectile, ECR_Ignore);
	SetRootComponent(CollisionBox);
	
	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MainMesh->SetupAttachment(CollisionBox);
	
	ShipMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("ShipMovementComponent"));
	check(ShipMovementComponent);

	// Constrain movement to XY plane
	ShipMovementComponent->SetPlaneConstraintEnabled(true);
	ShipMovementComponent->ConstrainNormalToPlane(GetActorUpVector());
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();
}

void AShip::Move(const FInputActionValue& Value)
{
	const float MoveInput = Value.Get<float>();

	AddMovementInput(GetActorRightVector(), MoveInput);
}

void AShip::Fire(const FInputActionValue& Value)
{
	if (!bCanFire) return;
	
	if (UPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPoolSubsystem>())
	{
		const FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 50.f;
		FPoolSpawnParameters SpawnParams;
		SpawnParams.ActorClass = ProjectileClass;
		SpawnParams.SpawnTransform = FTransform(FRotator::ZeroRotator, SpawnLocation);
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.Lifetime = 5.f;
		PoolSubsystem->SpawnActorFromPool<AProjectile>(SpawnParams);
	}

	bCanFire = false;
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AShip::EnableFire, FireRate, false);
}

void AShip::EnableFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	bCanFire = true;
}

void AShip::SpawnExplosionFX()
{
	if (!ExplosionFX) nullptr;
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation());	
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputMappingContext)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
			
			if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				EnhancedInputComp->BindAction(FireAction,  ETriggerEvent::Started, this, &AShip::Fire);
				EnhancedInputComp->BindAction(MoveAction,  ETriggerEvent::Triggered, this, &AShip::Move);
			}
		}
	}

}

void AShip::ReceiveHit(int32 Item)
{
	SpawnExplosionFX();
	
	if (ASIGameMode* GM = Cast<ASIGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnPlayerHit();	
	}
}

