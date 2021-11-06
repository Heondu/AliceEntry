// Fill out your copyright notice in the Description page of Project Settings.


#include "AEBasicCharacter.h"
#include "AEAnimInstance.h"
#include "AEWeapon.h"

AAEBasicCharacter::AAEBasicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAEBasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = Cast<UAEAnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != AnimInstance);

	AnimInstance->OnHitEnd.AddLambda([this]() -> void {
		CanMove = true;
		});

	Health = MaxHealth;

	FName WeaponSocket(TEXT("hand_rSocket"));
	AAEWeapon* NewWeapon = GetWorld()->SpawnActor<AAEWeapon>(Weapon);
	if (nullptr != NewWeapon)
	{
		//AAEWeapon Ŭ������ �����Ϸ��� ��������� �߰��ؾ� �Ѵ�.
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
}

void AAEBasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
