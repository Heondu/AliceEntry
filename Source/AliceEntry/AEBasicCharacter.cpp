// Fill out your copyright notice in the Description page of Project Settings.


#include "AEBasicCharacter.h"
#include "AEPlayerAnimInstance.h"
#include "AEWeapon.h"

AAEBasicCharacter::AAEBasicCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsDead = false;
}

void AAEBasicCharacter::BeginPlay()
{
	Super::BeginPlay();

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
