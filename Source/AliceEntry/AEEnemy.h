// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliceEntry.h"
#include "AEBasicCharacter.h"
#include "GameFramework/Character.h"
#include "AEEnemy.generated.h"

UCLASS()
class ALICEENTRY_API AAEEnemy : public AAEBasicCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAEEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION(BlueprintCallable, Category = Attack)
	void Attack();

private:
	void AttackCheck();
	void AttackMove();

	UPROPERTY()
	class UAEEnemyAnimInstance* AnimInstance;

	UPROPERTY()
	class AAEAIController* AIController;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackMoveForce;
};
