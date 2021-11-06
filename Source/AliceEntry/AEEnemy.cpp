// Fill out your copyright notice in the Description page of Project Settings.


#include "AEEnemy.h"
#include "AEAIController.h"
#include "AEAnimInstance.h"

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

	MaxCombo = 3;
}

void AAEEnemy::Attack()
{
	IsAttacking = true;
	PlayAnimMontage(AttackMontages[FMath::RandRange(0, AttackMontages.Num() - 1)], 1.0f);
}

// Called every frame
void AAEEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAEEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CHECK(nullptr != AnimInstance);
	AnimInstance->OnAttackHitCheck.AddUObject(this, &AAEEnemy::AttackCheck);
	AnimInstance->OnAttackEnd.AddLambda([this]() -> void {
		IsAttacking = false;
		});
	AnimInstance->OnHitAnimEnd.AddLambda([this]() -> void {
		AAEAIController* AIController = Cast<AAEAIController>(GetController());
		AIController->RunAI();
		});
}

// Called to bind functionality to input
void AAEEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAEEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	AAEAIController* AIController = Cast<AAEAIController>(GetController());
	AIController->StopAI();

	if (Health <= 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			TArray<AActor*> Actors;
			GetAttachedActors(Actors);
			for (AActor* Actor : Actors)
				Actor->Destroy();
			Destroy();
			}), DeadTimer, false);
	}
	return DamageApplied;
}