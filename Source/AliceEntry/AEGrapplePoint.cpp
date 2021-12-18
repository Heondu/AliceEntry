// Fill out your copyright notice in the Description page of Project Settings.


#include "AEGrapplePoint.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAEGrapplePoint::AAEGrapplePoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxImageSize = 50;
	MinImageSize = 10;
}

// Called when the game starts or when spawned
void AAEGrapplePoint::BeginPlay()
{
	Super::BeginPlay();

	DetectionComp = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("Detection")));
	DeactivateZoneComp = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("DeactivateZone")));
	DeactivateZoneComp->OnComponentBeginOverlap.AddDynamic(this, &AAEGrapplePoint::OnOverlapBegin);
	DeactivateZoneComp->OnComponentEndOverlap.AddDynamic(this, &AAEGrapplePoint::OnOverlapEnd);

	WidgetRef = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("Widget")))->GetUserWidgetObject();
	WidgetRef->SetVisibility(ESlateVisibility::Hidden);
}

// Called every frame
void AAEGrapplePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive && !bUsed)
	{
		CheckDistanceFromPlayer();
	}
}

void AAEGrapplePoint::Activate(class AAEPlayerCharacter* Player, bool _bCanGrapple)
{
	if (!bUsed)
	{
		PlayerRef = Player;
		bActive = true;
		WidgetRef->SetVisibility(ESlateVisibility::Visible);
		bCanGrapple = _bCanGrapple;
	}
}

void AAEGrapplePoint::Deactivate()
{
	if (!bUsed)
	{
		bActive = false;
		WidgetRef->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AAEGrapplePoint::CheckDistanceFromPlayer()
{
	if (nullptr == PlayerRef) return;

	UImage* Image = WidgetRef->WidgetTree->FindWidget<UImage>("Filling");
	float Value = (PlayerRef->GetActorLocation() - GetActorLocation()).Size();
	float DesiredSize = UKismetMathLibrary::MapRangeClamped(Value, PlayerRef->GrappleThrowDistance, PlayerRef->DetectionRadius, MaxImageSize, MinImageSize);
	Image->SetBrushSize(FVector2D(DesiredSize, DesiredSize));
	if (bCanGrapple)
	{
		Image->SetColorAndOpacity(DesiredSize == MaxImageSize ? FLinearColor(1.0f, 0.0f, 0.3f) : FLinearColor::White);
	}
	else
	{
		Image->SetColorAndOpacity(FLinearColor::White);
	}
}

void AAEGrapplePoint::Use()
{
	bUsed = true;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AAEGrapplePoint::Reactive, 2.0f, false);
}

void AAEGrapplePoint::Reactive()
{
	bUsed = false;
	bCanGrapple = false;
}

void AAEGrapplePoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerRef)
	{
		DetectionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AAEGrapplePoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerRef)
	{
		DetectionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}