// Fill out your copyright notice in the Description page of Project Settings.


#include "AEEnemy.h"
#include "AEAIController.h"
#include "AEAnimInstance.h"
#include "DrawDebugHelpers.h"

// Sets default values
AAEEnemy::AAEEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAEEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAEAIController>(GetController());

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAEEnemy::AttackCheck);
	AnimInstance->OnAttackEnd.AddLambda([this]() -> void {
		IsAttacking = false;
		});
	AnimInstance->OnHitEnd.AddLambda([this]() -> void {
		if (!AnimInstance->IsDead)
			AIController->RunAI();
		});

	//AIController는 블루프린트나 인스펙터의 Pawn 카테고리에서 설정 가능
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	//부드러운 회전을 위해 꺼야 함
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	//회전 보간
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
	
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	AttackRange = 200.0f;
	AttackRadius = 50.0f;
	IsAttacking = false;

	DeadTimer = 5.0f;

	CanMove = true;
}

// Called every frame
void AAEEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAEEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called to bind functionality to input
void AAEEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAEEnemy::Attack()
{
	IsAttacking = true;
	PlayAnimMontage(AttackMontages[FMath::RandRange(0, AttackMontages.Num() - 1)], 1.0f);
}

void AAEEnemy::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 1.0f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			LOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(Damage, DamageEvent, GetController(), this);
		}
	}
}

float AAEEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;

	LOG(Warning, TEXT("Health left %f"), Health);

	AIController->StopAI();

	if (Health <= 0.0f)
	{
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			TArray<AActor*> Actors;
			GetAttachedActors(Actors);
			for (AActor* Actor : Actors)
				Actor->Destroy();
			Destroy();
			}), DeadTimer, false);
	}
	else
	{
		CanMove = false;
		IsAttacking = false;
		AnimInstance->PlayHitAnim();
	}

	return DamageApplied;
}