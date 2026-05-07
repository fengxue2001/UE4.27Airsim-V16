//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "Components/BoxComponent.h"
//#include "Checkpoint.generated.h"
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCheckpointClearedDelegate, int32, NextCheckpoint);
//
//UCLASS()
//class AIRSIM_API UCheckpointPose : public UObject
//{
//	GENERATED_BODY()
//public:
//	FVector Position;
//	FRotator Orientation;
//};
//
//UCLASS()
//class AIRSIM_API ACheckpoint : public AActor
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this actor's properties
//	ACheckpoint();
//
//	UPROPERTY(BlueprintAssignable)
//	FCheckpointClearedDelegate OnCheckpointCleared;
//
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	UFUNCTION()
//	void OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
//
//	UPROPERTY(EditAnywhere)
//	UParticleSystemComponent* ParticleSystem;
//
//	UPROPERTY(EditAnywhere)
//	int32 CheckpointNumber;
//
//	UPROPERTY(EditAnywhere)
//	UBoxComponent* Trigger;
//
//	UPROPERTY(EditAnywhere)
//	UStaticMeshComponent* SphereVisual;
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//};
