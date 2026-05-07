//// Fill out your copyright notice in the Description page of Project Settings.
//#include "Checkpoint.h"
//
//#include "Components/SphereComponent.h"
//#include "Components/ArrowComponent.h"
//#include "Components/BoxComponent.h"
//#include "Particles/ParticleSystemComponent.h"
//#include "Vehicles/Multirotor/FlyingPawn.h"
//
//
//// Sets default values
//ACheckpoint::ACheckpoint()
//{
//	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
//	RootComponent = Root;
//
//	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
//	Trigger->SetupAttachment(RootComponent);
//	Trigger->SetWorldScale3D(FVector(1.5f));
//	Trigger->SetVisibility(false);
//	Trigger->SetHiddenInGame(false);
//	Trigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
//	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlap);
//
//	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MovementParticles"));
//	ParticleSystem->SetupAttachment(RootComponent);
//
//	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
//	SphereVisual->SetupAttachment(RootComponent);
//	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("StaticMesh'/AirSim/Models/Checkpoint/source/gold_star.gold_star'"));
//	if (SphereVisualAsset.Succeeded())
//	{
//		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
//		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
//		SphereVisual->SetWorldScale3D(FVector(0.025f));
//		SphereVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	}
//}
//
//// Called when the game starts or when spawned
//void ACheckpoint::BeginPlay()
//{
//	Super::BeginPlay();
//}
//
//// Called every frame
//void ACheckpoint::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	FVector NewLocation = GetActorLocation();
//	FRotator NewRotation = GetActorRotation();
//	float RunningTime = GetGameTimeSinceCreation();
//	float DeltaRotation = DeltaTime * 20.0f;	//Rotate by 20 degrees per second
//	NewRotation.Yaw += DeltaRotation;
//	SetActorLocationAndRotation(NewLocation, NewRotation);
//}
//
//void ACheckpoint::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	AFlyingPawn* FlyingPawn = Cast<AFlyingPawn>(OtherActor);
//
//	if (FlyingPawn) {
//		OnCheckpointCleared.Broadcast(CheckpointNumber);
//	}
//}
//
