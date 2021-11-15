// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "GameFramework/Character.h"
#include "AEBasicCharacter.generated.h"

UCLASS()
class ALICEENTRY_API AAEBasicCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAEBasicCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bIsAttacking;

protected:
	//Ŭ������ �������Ʈ�� �޾ƿ����� TSubclassOf�� Ŭ������ ����ؾ� �Ѵ�.
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class AAEWeapon> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = { };

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere)
	float Damage = 10;

	UPROPERTY(VisibleAnywhere)
	bool bCanMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsDead;
};