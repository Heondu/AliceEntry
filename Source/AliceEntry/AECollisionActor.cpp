// Fill out your copyright notice in the Description page of Project Settings.


#include "AECollisionActor.h"
#include "AECharacter.h"

// Sets default values
AAECollisionActor::AAECollisionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = mStaticMesh;

	//CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	//CollisionSphere->InitSphereRadius(100.0f);
	//CollisionSphere->SetupAttachment(RootComponent);
	//
	//CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAECollisionActor::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	ParticleFX = ParticleAsset.Object;
}

// Called when the game starts or when spawned
void AAECollisionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAECollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AAECollisionActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleFX, GetActorLocation());
	Destroy();
	return FinalDamage;
}

//void AAECollisionActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor->IsA(AAECharacter::StaticClass()))
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Collision Touch!"));
//
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleFX, GetActorLocation());
//		Destroy();
//	}
//}

