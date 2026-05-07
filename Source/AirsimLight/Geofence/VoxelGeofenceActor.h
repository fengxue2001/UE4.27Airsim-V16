#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelGeofenceActor.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;

class ALidarPointCloudActor;
class ULidarPointCloud;
struct FLidarPointCloudPoint;

UCLASS()
class AIRSIMLIGHT_API AVoxelGeofenceActor : public AActor
{
    GENERATED_BODY()

public:
    AVoxelGeofenceActor();

protected:
    virtual void BeginPlay() override;

private:
    // 延迟/重试入口
    void ScheduleBuild(float DelaySeconds);
    void BuildVoxelGrid();

    // 点云路径
    ALidarPointCloudActor* FindPointCloudActor() const;
    void BuildFromPointCloud(ALidarPointCloudActor* PCA);
    void RenderFromOccupiedVoxels(const TSet<FIntVector>& Occupied, const FVector& OriginWS);

    // 从点结构提取位置（字段名不一致时，只改这里）
    FVector ExtractPointWorldLocation(const FLidarPointCloudPoint& P) const;

private:
    UPROPERTY(VisibleAnywhere, Category = "VoxelFence")
    UInstancedStaticMeshComponent* InstancedMesh = nullptr;

    // timer handle 必须是成员
    FTimerHandle BuildTimerHandle;

    // 如果点云尚未就绪（或读点失败），重试次数
    int32 BuildRetryCount = 0;

public:
    // ---------- 可在 Details 面板配置 ----------
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Render")
    UStaticMesh* VoxelMesh = nullptr;

    UPROPERTY(EditAnywhere, Category = "VoxelFence|Render")
    UMaterialInterface* VoxelMaterial = nullptr;

    // UE单位cm：100cm=1m
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Voxel", meta = (ClampMin = "1.0"))
    float VoxelSize = 100.f;

    UPROPERTY(EditAnywhere, Category = "VoxelFence|Voxel", meta = (ClampMin = "1"))
    int32 MaxTotalVoxels = 100000;

    // 一次读点批大小（建议先小后大）
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Voxel", meta = (ClampMin = "1000"))
    int32 BatchSize = 10000;

    // 默认只画边界壳
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Voxel")
    bool bShowOnlyBoundary = true;

    // 用 Tag 精准匹配点云 Actor（你填 Substation4）
    UPROPERTY(EditAnywhere, Category = "VoxelFence|PointCloud")
    FName PointCloudActorTag = NAME_None;

    // 构建延迟（秒），用于等待点云 Octree/Streaming 就绪
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Runtime", meta = (ClampMin = "0.0"))
    float InitialBuildDelay = 0.2f;

    // 读点失败时的重试延迟（秒）
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Runtime", meta = (ClampMin = "0.01"))
    float RetryDelay = 0.3f;

    // 最多重试次数
    UPROPERTY(EditAnywhere, Category = "VoxelFence|Runtime", meta = (ClampMin = "0"))
    int32 MaxRetries = 5;
};
