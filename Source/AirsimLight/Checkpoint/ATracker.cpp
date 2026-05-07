//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "Tracker.h"
//#include "Components/BillboardComponent.h"
//#include "Sound/SoundCue.h"
//#include "Particles/ParticleSystemComponent.h"
//#include "Kismet/GameplayStatics.h"
//#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"
//#include "common/Common.hpp"
//#include "common/CommonStructs.hpp"
//
//
//using msr::airlib::Vector3r;
//
//
//// Sets default values
//ATracker::ATracker()
//	:AutopilotSpeed(4.0),
//	DefaultBestLap(120),
//	MaximumSkewThreshold(0.25),
//	ProgressWeight(1.0f),
//	ProgressHist(0.0f),
//	SkewWeight(1e-3f),
//	SkewHist(0.0f),
//	ControlWeight(2.5e-3f),
//	ControlHist(0.0f),
//	AttentionWeight(1e-3f),
//	AttentionHist(0.0f),
//	TimeWeight(1e-1f),
//	PathLength(0.1f),
//	SendCommandAutopilot(true)
//{
//	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	UBillboardComponent* BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("RootComponent"));
//	RootComponent = BillboardComponent;
//
//	CheckpointSoundFX = CreateDefaultSubobject<UAudioComponent>(TEXT("CheckpointSoundFX"));
//	CheckpointSoundFX->SetupAttachment(RootComponent);
//	static ConstructorHelpers::FObjectFinder<USoundBase> loadedCheckpointSoundFX(TEXT("/Game/Audio/Blueprints_TextPop03.Blueprints_TextPop03"));
//	if (loadedCheckpointSoundFX.Object != NULL) {
//		CheckpointSoundFX->SetSound(loadedCheckpointSoundFX.Object);
//	}
//
//}
//
//// Called when the game starts or when spawned
//void ATracker::BeginPlay()
//{
//	Super::BeginPlay();
//	GenerateGeofenceCorridorFromPath(); // 原本路径生成的飞行走廊保留（如果你不要，告诉我可删掉）
//}
//
//
//// Called every frame
//void ATracker::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// 空指针检查，避免崩溃
//	if (!SimMode || !ControllerReference) {
//		return;
//	}
//
//	UpdateScore();
//	UpdateMode();
//
//	static bool enable_autopilot_previous = false;
//
//	if (ControllerReference->AutopilotCheck()) {
//		// 自动驾驶初始化逻辑，只执行一次
//		if (!enable_autopilot_previous) {
//			SendCommandAutopilot = true;
//			client.confirmConnection();
//			client.enableApiControl(true);
//			client.armDisarm(true);
//		}
//		Autopilot();
//		enable_autopilot_previous = true;
//	}
//	else {
//		// 非自动驾驶状态
//		SendCommandAutopilot = false;
//		enable_autopilot_previous = false;
//		client.enableApiControl(false);
//	}
//}
//
//
//
//void ATracker::ReadPointList(std::string filename) {
//	std::ifstream fin(filename, std::ios::in);
//
//	int32 num_points;
//	fin >> num_points;
//
//	for (int32 i = 0; i < num_points; i++) {
//
//		float x, y, z;
//		fin >> x >> y >> z;
//
//		UCheckpointPose* pose = NewObject<UCheckpointPose>();
//		pose->Position = 100 * FVector(x, -y, z);
//		CheckpointPoses.Push(pose);
//	}
//
//	fin.close();
//}
//
//void ATracker::SpawnCheckpoints()
//{
//	UWorld* World = GetWorld();
//	if (!World) return;
//
//	FString LevelName = World->GetMapName();
//	LevelName.RemoveFromStart(World->StreamingLevelsPrefix); // 去掉前缀
//
//	UE_LOG(LogTemp, Warning, TEXT("当前Level名称: %s"), *LevelName);
//
//	// 构建不同关卡对应的路径（你可以更细化地匹配）
//	std::string points_file;
//
//	if (LevelName == "DJS1") {
//		points_file = "/Config/points2.txt";
//		UE_LOG(LogTemp, Warning, TEXT("DJS1：%s —— 已接入该地图对应的巡检点！"), *LevelName);
//	}
//	else if (LevelName == "XLC") {
//		points_file = "/Config/points3.txt";
//		UE_LOG(LogTemp, Warning, TEXT("XLC：%s —— 已接入该地图对应的巡检点！"), *LevelName);
//	}
//	else {
//		points_file = "/Config/points.txt"; // 默认 fallback
//		UE_LOG(LogTemp, Warning, TEXT("XL：%s —— 已接入该地图对应的巡检点！"), *LevelName);
//	}
//
//	// 构建绝对路径并读取
//	FString ProjectPath = FPaths::GetPath(FPaths::GetProjectFilePath());
//	std::string full_path = TCHAR_TO_UTF8(*ProjectPath) + points_file;
//
//	ReadPointList(full_path);
//
//	TotalCheckpoints = CheckpointPoses.Num();
//	NextCheckpoint = 0;
//}
//
//
//void ATracker::SetSimMode(ASimModeBase* sim_mode) {
//	SimMode = sim_mode;
//}
//
//void ATracker::LapCheck(int32 CheckpointNumber) {
//
//	if (CheckpointNumber == TotalCheckpoints - 1) {
//		ControllerReference->LapTimeCheck();
//	}
//	else {
//		NextCheckpoint = CheckpointNumber + 1;
//		ActivateCheckpoint(NextCheckpoint);
//	}
//
//}
//
//void ATracker::ActivateCheckpoint(int32 CheckpointNumber) {
//
//	SendCommandAutopilot = true;
//
//	if (CheckpointNumber >= TotalCheckpoints) {
//		return;
//	}
//
//	if (Checkpoint) {
//		DeactiveCheckpoint();
//	}
//
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.Owner = this;
//	SpawnParams.Instigator = GetInstigator();
//	UWorld* World = GetWorld();
//	if (World) {
//		FVector Position = CheckpointPoses[CheckpointNumber]->Position;
//		Checkpoint = World->SpawnActor<ACheckpoint>(Position, FRotator::ZeroRotator, SpawnParams);
//
//		Checkpoint->CheckpointNumber = CheckpointNumber;
//		Checkpoint->OnCheckpointCleared.AddDynamic(this, &ATracker::CheckpointCleared);
//	}
//}
//
//void ATracker::StartSequence() {
//	ActivateCheckpoint(0);
//}
//
//void ATracker::DeactiveCheckpoint()
//{
//	if (Checkpoint) {
//		CheckpointSoundFX->SetWorldLocation(Checkpoint->GetActorLocation());
//		CheckpointSoundFX->Play();
//		Checkpoint->Destroy();
//	}
//}
//
//void ATracker::CheckpointCleared(int CheckpointNumber) {
//
//	DeactiveCheckpoint();
//
//	LapCheck(CheckpointNumber);
//}
//
//void ATracker::CalculateProgressWeight() {
//
//	PathLength = 0.0f;
//
//	auto Norm = [](FVector v)->float {
//		return sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
//		};
//
//	AccumulateProgress.Push(0.0f);
//	for (int32 i = 0; i < TotalCheckpoints - 1; i++) {
//		// cm to m
//		FVector start = CheckpointPoses[i]->Position / 100;
//		FVector end = CheckpointPoses[i + 1]->Position / 100;
//		PathLength += Norm(end - start);
//		AccumulateProgress.Push(PathLength);
//	}
//	ProgressWeight = 100.0f / PathLength;
//}
//
//
//float ATracker::CalculateTrajectoryScore() {
//
//	if (!SimMode) {
//		return 0;
//	}
//
//	// cm to m
//	FVector PreCheckpointPosition = CheckpointPoses[NextCheckpoint - 1]->Position / 100;
//	FVector NextCheckpointPosition = CheckpointPoses[NextCheckpoint]->Position / 100;
//
//	Eigen::Vector3f v_normal(NextCheckpointPosition.X - PreCheckpointPosition.X, NextCheckpointPosition.Y - PreCheckpointPosition.Y, NextCheckpointPosition.Z - PreCheckpointPosition.Z);
//	v_normal = v_normal.normalized();
//
//	APawn* Pawn = Cast<AFlyingPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
//	FVector PawnPosition = Pawn->GetActorLocation() / 100;
//
//	Eigen::Vector3f v_quad(PawnPosition.X - PreCheckpointPosition.X, PawnPosition.Y - PreCheckpointPosition.Y, PawnPosition.Z - PreCheckpointPosition.Z);
//
//	float progress = v_quad.dot(v_normal);
//	progress = fmaxf(0, fminf(progress, (PreCheckpointPosition - NextCheckpointPosition).Size()));
//
//	float skew = (v_quad - progress * v_normal).norm();
//
//	ProgressHist = AccumulateProgress[NextCheckpoint - 1] + progress;
//
//	float ProgressScore = ProgressWeight * ProgressHist;
//
//	ProgressScore = fmaxf(0, ProgressScore);
//
//	if (skew >= MaximumSkewThreshold) {
//		SkewHist += skew - MaximumSkewThreshold;
//	}
//
//	float SkewScore = -SkewWeight * SkewHist;
//
//	float TrajectoryScore = ProgressScore + SkewScore;
//
//	return TrajectoryScore;
//}
//
//float ATracker::CalculateControlScore() {
//
//	msr::airlib::RCData data = SimMode->getRCData();
//
//	float roll = data.roll;
//	float pitch = data.pitch;
//	float yaw = data.yaw;
//	float throttle = data.throttle;
//
//	ControlHist += roll * roll + pitch * pitch + yaw * yaw + throttle * throttle;
//
//	float ControlScore = -ControlWeight * ControlHist;
//
//	return ControlScore;
//}
//
//float ATracker::CalculateAttentionScore() {
//	//TODO: TO BE IMPLEMENTED
//	return 0;
//}
//
//float ATracker::CalculateTimeScore() {
//	float TimeScore = -TimeWeight * ControllerReference->GetLapTime();
//	return TimeScore;
//}
//
//void ATracker::UpdateScore() {
//
//	if (!SimMode || ControllerReference->RaceCompleteCheck() || NextCheckpoint == 0) {
//		return;
//	}
//
//	float traj_score = CalculateTrajectoryScore();
//	float ctrl_score = CalculateControlScore();
//	float attn_score = CalculateAttentionScore();
//	float time_score = CalculateTimeScore();
//
//	float CurrentScore = traj_score + ctrl_score + attn_score + time_score;
//
//	// 自动飞行加 100 分
//	if (ControllerReference->AutopilotCheck()) {
//		CurrentScore += 100.0f;
//	}
//
//	UE_LOG(LogTemp, Log, TEXT("分数明细：Trajectory=%.2f, Control=%.2f, Attention=%.2f, Time=%.2f, Total=%.2f"),
//		traj_score, ctrl_score, attn_score, time_score, CurrentScore);
//
//	ControllerReference->UpdateScore(CurrentScore);
//}
//
//
//void ATracker::UpdateMode() {
//
//	msr::airlib::RCData data = SimMode->getRCData();
//
//	float angle_mode = static_cast<float>(data.getSwitch(0));
//
//	if (angle_mode < 0.33)
//		ControllerReference->SetControlMode(0);
//	else if (angle_mode < 0.66)
//		ControllerReference->SetControlMode(1);
//	else
//		ControllerReference->SetControlMode(2);
//
//}
//
//
//void ATracker::Autopilot() {
//	if (!SimMode || !SendCommandAutopilot) {
//		return;
//	}
//
//	std::vector<Eigen::Vector3f> path;
//	for (int32 i = NextCheckpoint; i < TotalCheckpoints; i++) {
//		FVector pos = CheckpointPoses[NextCheckpoint]->Position;
//		Eigen::Vector3f wp(pos.X / 100, pos.Y / 100, -pos.Z / 100);
//		// 纠正起始点偏移
//		wp -= Eigen::Vector3f(-17, -47, -0.5);
//		path.push_back(wp);
//	}
//	client.moveOnPathAsync(
//		path,
//		AutopilotSpeed,
//		1e3,
//		msr::airlib::DrivetrainType::ForwardOnly,
//		msr::airlib::YawMode(false, 0),
//		1.0f,
//		1.0f
//	);
//
//	SendCommandAutopilot = false;
//}
//
//void ATracker::GenerateGeofenceCorridorFromPath()
//{
//	if (!GeofenceBlueprint || CheckpointPoses.Num() < 2) return;
//
//	const float CorridorWidth = 200.0f; // 屏障宽度（从路径中心左右各拓展）
//	const float SegmentResolution = 200.0f; // 每段路径插值间隔（越小越密）
//
//	for (int32 i = 0; i < CheckpointPoses.Num() - 1; ++i)
//	{
//		FVector Start = CheckpointPoses[i]->Position;
//		FVector End = CheckpointPoses[i + 1]->Position;
//
//		FVector Direction = (End - Start).GetSafeNormal();
//		FVector Right = FVector::CrossProduct(FVector::UpVector, Direction);
//
//		float SegmentLength = FVector::Dist(Start, End);
//		int32 NumSamples = FMath::CeilToInt(SegmentLength / SegmentResolution);
//
//		for (int32 j = 0; j <= NumSamples; ++j)
//		{
//			FVector Center = FMath::Lerp(Start, End, static_cast<float>(j) / NumSamples);
//
//			// 左右两侧偏移后的位置
//			TArray<FVector> SpawnPositions = {
//				Center + Right * CorridorWidth * 0.5f,
//				Center - Right * CorridorWidth * 0.5f
//			};
//
//			for (const FVector& Location : SpawnPositions)
//			{
//				// 可选：避免重复生成
//				FVector GridPos = FVector(
//					FMath::GridSnap(Location.X, 100.0f),
//					FMath::GridSnap(Location.Y, 100.0f),
//					FMath::GridSnap(Location.Z, 100.0f)
//				);
//
//				// 碰撞检测
//				FHitResult HitResult;
//				bool bHit = GetWorld()->SweepSingleByChannel(
//					HitResult,
//					GridPos,
//					GridPos,
//					FQuat::Identity,
//					ECC_WorldStatic,
//					FCollisionShape::MakeBox(FVector(50, 50, 50))
//				);
//
//				if (!bHit)
//				{
//					FActorSpawnParameters Params;
//					Params.Owner = this;
//					AActor* Fence = GetWorld()->SpawnActor<AActor>(
//						GeofenceBlueprint,
//						GridPos,
//						FRotator::ZeroRotator,
//						Params
//					);
//					if (Fence)
//					{
//						Fence->SetLifeSpan(10.0f); // 可调整
//					}
//				}
//			}
//		}
//	}
//}
//
//
//void ATracker::SpawnGeofenceAtLocation(const FVector& Location)
//{
//	if (!GeofenceBlueprint) {
//		UE_LOG(LogTemp, Warning, TEXT("Geofence Blueprint is not assigned!"));
//		return;
//	}
//
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.Owner = this;
//
//	FVector SpawnLocation = Location;
//	FRotator SpawnRotation = FRotator::ZeroRotator;
//
//	AActor* Obstacle = GetWorld()->SpawnActor<AActor>(GeofenceBlueprint, SpawnLocation, SpawnRotation, SpawnParams);
//
//	if (Obstacle) {
//		Obstacle->SetLifeSpan(10.0f); // 自动销毁，避免堆积
//	}
//}
