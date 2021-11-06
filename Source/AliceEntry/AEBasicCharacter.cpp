// Fill out your copyright notice in the Description page of Project Settings.


#include "AEBasicCharacter.h"
#include "AEAnimInstance.h"
#include "AEWeapon.h"
#include "DrawDebugHelpers.h"

AAEBasicCharacter::AAEBasicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	IsAttacking = false;
	AttackEndComboState();

	DeadTimer = 5.0f;

	canMove = true;
}

void AAEBasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	FName WeaponSocket(TEXT("hand_rSocket"));
	AAEWeapon* NewWeapon = GetWorld()->SpawnActor<AAEWeapon>(Weapon);
	if (nullptr != NewWeapon)
	{
		//AAEWeapon 클래스에 접근하려면 헤더파일을 추가해야 한다.
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}
}

void AAEBasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAEBasicCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInstance = Cast<UAEAnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != AnimInstance);

	AnimInstance->OnHitAnimEnd.AddLambda([this]() -> void {
		canMove = true;
		});
}

void AAEBasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAEBasicCharacter::Attack()
{
	if (!canMove) return;

	if (IsAttacking)
	{
		CHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		CHECK(CurrentCombo == 0);
		AttackStartComboState();
		AnimInstance->PlayAttackMontage();
		AnimInstance->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AAEBasicCharacter::OnAttackMontageEnded()
{
	CHECK(IsAttacking);
	CHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	//OnAttackEnd.Broadcast();
}

void AAEBasicCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AAEBasicCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

float AAEBasicCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;

	LOG(Warning, TEXT("Health left %f"), Health);

	if (Health <= 0.0f)
	{
		AnimInstance->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	else
	{
		canMove = false;
		IsAttacking = false;
		AttackEndComboState();
		AnimInstance->PlayHitAnim();
	}
	return DamageApplied;
}

void AAEBasicCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_EngineTraceChannel4,
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