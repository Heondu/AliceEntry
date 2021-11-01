// Fill out your copyright notice in the Description page of Project Settings.


#include "AEEnemy.h"
#include "AEAIController.h"
#include "AEAnimInstance.h"

// Sets default values
AAEEnemy::AAEEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AIController�� �������Ʈ�� �ν������� Pawn ī�װ����� ���� ����
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//�ε巯�� ȸ���� ���� ���� ��
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	//ȸ�� ����
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	AttackRange = 200.0f;
	AttackRadius = 50.0f;

	MaxCombo = 3;
}

// Called when the game starts or when spawned
void AAEEnemy::BeginPlay()
{
	Super::BeginPlay();

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
}

// Called to bind functionality to input
void AAEEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAEEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Health <= 0.0f)
	{
		AAEAIController* AIController = Cast<AAEAIController>(GetController());
		AIController->StopAI();

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
			Destroy();
			}), DeadTimer, false);
	}
	return DamageApplied;
}