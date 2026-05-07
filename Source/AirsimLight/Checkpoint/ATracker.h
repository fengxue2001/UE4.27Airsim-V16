//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//#include "Engine/World.h"
//#include "TimerManager.h"
//#include "Components/AudioComponent.h"
//#include "FLPlayerController.h"
//#include "Checkpoint.h"
//#include "SimMode/SimModeBase.h"
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
//#include "Tracker.generated.h"
//
//UCLASS()
//class AIRSIM_API ATracker : public AActor
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this actor's properties
//	ATracker();
//
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	void ReadPointList(std::string FileName);
//
//	UFUNCTION()
//	void SpawnCheckpoints();
//
//	UFUNCTION()
//	void SetSimMode(ASimModeBase* sim_mode);
//
//	UFUNCTION()
//	void LapCheck(int32 CheckpointNumber);
//
//	UFUNCTION()
//	void ActivateCheckpoint(int32 CheckpointNumber);
//
//	UFUNCTION()
//	void StartSequence();
//
//	UFUNCTION()
//	void DeactiveCheckpoint();
//
//	UFUNCTION()
//	void CheckpointCleared(int CheckpointNumber);
//
//	UFUNCTION()
//	void CalculateProgressWeight();
//
//	UFUNCTION()
//	float CalculateTrajectoryScore();
//
//	UFUNCTION()
//	float CalculateControlScore();
//
//	UFUNCTION()
//	float CalculateAttentionScore();
//
//	UFUNCTION()
//	float CalculateTimeScore();
//
//	UFUNCTION()
//	void UpdateScore();
//
//	UFUNCTION()
//	void UpdateMode();
//
//	UFUNCTION()
//	void Autopilot();
//	// 手动或自动生成地理围栏（飞行屏障）
//	UFUNCTION()
//	void SpawnGeofenceAtLocation(const FVector& Location);
//
//	void GenerateGeofenceCorridorFromPath();
//
//
//private:
//
//	UPROPERTY(EditAnywhere, Category = "Custom Settings")
//	float AutopilotSpeed;
//
//	UPROPERTY(EditAnywhere, Category = "Custom Settings")
//	float DefaultBestLap;
//
//	UPROPERTY(EditAnywhere, Category = "Custom Settings")
//	UAudioComponent* CheckpointSoundFX;
//
//	UPROPERTY(EditAnywhere, Category = "Checkpoints")
//	ACheckpoint* Checkpoint;
//
//	UPROPERTY(EditAnywhere, Category = "Checkpoints")
//	TArray<UCheckpointPose*> CheckpointPoses;
//
//	UPROPERTY(EditAnywhere, Category = "Custom Settings")
//	int32 TotalCheckpoints;
//
//	UPROPERTY(EditAnywhere, Category = "Custom Settings")
//	int32 NextCheckpoint;
//
//	UPROPERTY(EditAnywhere)
//	AFLPlayerController* ControllerReference;
//
//	UPROPERTY(EditAnywhere)
//	ASimModeBase* SimMode;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float MaximumSkewThreshold;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float ProgressWeight;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float ProgressHist;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float SkewWeight;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float SkewHist;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float ControlWeight;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float ControlHist;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float AttentionWeight;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float AttentionHist;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float TimeWeight;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	float PathLength;
//
//	UPROPERTY(EditAnywhere, Category = "Score")
//	TArray<float> AccumulateProgress;
//
//	msr::airlib::MultirotorRpcLibClient client;
//
//	UPROPERTY(EditAnywhere)
//	bool SendCommandAutopilot;
//
//	/** 地理围栏蓝图类引用（用于动态生成） */
//	UPROPERTY(EditAnywhere, Category = "Geofence")
//	TSubclassOf<AActor> GeofenceBlueprint;
//
//	/** 生成飞行围栏的时间间隔（秒） */
//	UPROPERTY(EditAnywhere, Category = "Geofence")
//	float GeofenceSpawnInterval = 1.0f;
//
//	/** 用于定时器的句柄 */
//	FTimerHandle GeofenceTimerHandle;
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//};