#include "AirsimLight/Geofence/VoxelGeofenceActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Materials/MaterialInterface.h"

// LiDAR Point Cloud plugin
#include "LidarPointCloudActor.h"
#include "LidarPointCloud.h"
#include "LidarPointCloudComponent.h"

AVoxelGeofenceActor::AVoxelGeofenceActor()
{
    PrimaryActorTick.bCanEverTick = false;

    bShowOnlyBoundary = true;
    VoxelSize = 25.0f;
    MaxTotalVoxels = 800000;
    InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
    SetRootComponent(InstancedMesh);

    InstancedMesh->SetMobility(EComponentMobility::Movable);
    InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVoxelGeofenceActor::BeginPlay()
{
    Super::BeginPlay();

    if (InstancedMesh)
    {
        InstancedMesh->ClearInstances();
        InstancedMesh->SetVisibility(true, true);
        InstancedMesh->SetHiddenInGame(false);
        InstancedMesh->SetCastShadow(false);
    }

    UE_LOG(LogTemp, Warning, TEXT("VoxelGeofence BeginPlay: VoxelMesh=%s, VoxelMaterial=%s"),
        VoxelMesh ? *VoxelMesh->GetName() : TEXT("NULL"),
        VoxelMaterial ? *VoxelMaterial->GetName() : TEXT("NULL"));

    if (!VoxelMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("VoxelGeofenceActor: VoxelMesh is NULL. Assign Engine/BasicShapes/Cube in Details."));
        return;
    }

    InstancedMesh->SetStaticMesh(VoxelMesh);

    if (VoxelMaterial)
        InstancedMesh->SetMaterial(0, VoxelMaterial);

    UE_LOG(LogTemp, Warning, TEXT("VoxelGeofenceActor: Schedule build after %.2fs (Tag=%s)"),
        InitialBuildDelay, *PointCloudActorTag.ToString());

    BuildRetryCount = 0;
    ScheduleBuild(InitialBuildDelay);
}

void AVoxelGeofenceActor::ScheduleBuild(float DelaySeconds)
{
    if (!GetWorld()) return;

    GetWorldTimerManager().ClearTimer(BuildTimerHandle);

    GetWorldTimerManager().SetTimer(
        BuildTimerHandle,
        this,
        &AVoxelGeofenceActor::BuildVoxelGrid,
        DelaySeconds,
        false
    );
}

void AVoxelGeofenceActor::BuildVoxelGrid()
{
    if (!GetWorld() || !InstancedMesh || VoxelSize <= KINDA_SMALL_NUMBER)
        return;

    if (!VoxelMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("VoxelGeofenceActor: No VoxelMesh assigned."));
        return;
    }

    ALidarPointCloudActor* PCA = FindPointCloudActor();
    if (!PCA)
    {
        UE_LOG(LogTemp, Warning, TEXT("VoxelGeofenceActor: No ALidarPointCloudActor found (Tag=%s)."),
            *PointCloudActorTag.ToString());
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Using PointCloudActor=%s"), *PCA->GetName());

    BuildFromPointCloud(PCA);
}

ALidarPointCloudActor* AVoxelGeofenceActor::FindPointCloudActor() const
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    ALidarPointCloudActor* First = nullptr;

    for (TActorIterator<ALidarPointCloudActor> It(World); It; ++It)
    {
        ALidarPointCloudActor* A = *It;
        if (!A) continue;

        if (!First) First = A;

        if (PointCloudActorTag != NAME_None)
        {
            if (A->ActorHasTag(PointCloudActorTag))
                return A;
        }
        else
        {
            return A;
        }
    }

    return First;
}

FVector AVoxelGeofenceActor::ExtractPointWorldLocation(const FLidarPointCloudPoint& P) const
{
    // ⚠️ 注意：bReturnWorldSpace=false 时，这里拿到的往往是“相对点云中心”的坐标（不是 ActorLocal）
    // 如果你本地字段不是 Location（例如 Position），只改这里一行即可
    const auto L = P.Location;
    return FVector((double)L.X, (double)L.Y, (double)L.Z);
}

void AVoxelGeofenceActor::BuildFromPointCloud(ALidarPointCloudActor* PCA)
{
    if (!PCA) return;

    ULidarPointCloud* Cloud = PCA->GetPointCloud();
    if (!Cloud)
    {
        if (ULidarPointCloudComponent* Comp = PCA->GetPointCloudComponent())
            Cloud = Comp->GetPointCloud();
    }
    if (!Cloud)
    {
        UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Cloud is null on %s"), *PCA->GetName());
        return;
    }

    const FBox BoundsWS = PCA->GetComponentsBoundingBox(true);
    if (!BoundsWS.IsValid)
    {
        UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Invalid bounds on %s"), *PCA->GetName());
        return;
    }

    // =========================
    // Step 1: 对齐自检日志（关键改动：用组件 Transform，不用 Actor Transform）
    // =========================
    ULidarPointCloudComponent* PCC = PCA->GetPointCloudComponent();
    const FTransform ActorTf = PCA->GetActorTransform();
    const FTransform CloudTf = (PCC ? PCC->GetComponentTransform() : ActorTf);

    // LocationOffset 在你这里是 FDoubleVector，需要显式转 FVector
    const auto OffsetD = Cloud->LocationOffset; // FDoubleVector
    const FVector CloudOffset(
        (float)OffsetD.X,
        (float)OffsetD.Y,
        (float)OffsetD.Z
    );

    UE_LOG(LogTemp, Warning, TEXT("[Align] PCA(ActorTf)=%s  Loc=%s  Rot=%s  Scale=%s"),
        *PCA->GetName(),
        *ActorTf.GetLocation().ToString(),
        *ActorTf.GetRotation().Rotator().ToString(),
        *ActorTf.GetScale3D().ToString());

    if (PCC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Align] PCC(CompTf) Loc=%s  Rot=%s  Scale=%s"),
            *CloudTf.GetLocation().ToString(),
            *CloudTf.GetRotation().Rotator().ToString(),
            *CloudTf.GetScale3D().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Align] PCA has no PointCloudComponent, fallback to ActorTf."));
    }

    UE_LOG(LogTemp, Warning, TEXT("[Align] BoundsWS Min=%s  Max=%s"),
        *BoundsWS.Min.ToString(), *BoundsWS.Max.ToString());

    UE_LOG(LogTemp, Warning, TEXT("[Align] Cloud LocationOffset(FDoubleVector)=(%.3f, %.3f, %.3f)  CloudOffset(FVector)=%s"),
        (double)OffsetD.X, (double)OffsetD.Y, (double)OffsetD.Z,
        *CloudOffset.ToString());

    // 体素原点：仍然用 world 的 bounds min
    const FVector OriginWS = BoundsWS.Min;

    UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Using PCA=%s BoundsMin=%s BoundsMax=%s VoxelSize=%.2f"),
        *PCA->GetName(), *BoundsWS.Min.ToString(), *BoundsWS.Max.ToString(), VoxelSize);

    // =========================
    // Step 2: 强制统一坐标链路（解决整体平移）
    // 查询坐标：Cloud-centered local（ComponentLocal - LocationOffset）
    // 点坐标：PointCentered + LocationOffset -> ComponentLocal -> World
    // =========================
    const FTransform CloudTfInv = CloudTf.Inverse();

    const float Tile = FMath::Max(80.0f * VoxelSize, 1000.0f); // cm
    const int32 MaxTiles = 2000;
    const int32 MaxPointsPerTile = 50000;

    TSet<FIntVector> Occupied;
    Occupied.Reserve(FMath::Min(MaxTotalVoxels, 300000) * 2);

    int32 TilesVisited = 0;
    int64 TotalPointsUsed = 0;

    TArray64<FLidarPointCloudPoint> TilePoints;

    bool bLoggedFirstPoint = false;

    for (float x = BoundsWS.Min.X; x < BoundsWS.Max.X; x += Tile)
    {
        for (float y = BoundsWS.Min.Y; y < BoundsWS.Max.Y; y += Tile)
        {
            for (float z = BoundsWS.Min.Z; z < BoundsWS.Max.Z; z += Tile)
            {
                if (TilesVisited++ >= MaxTiles)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Reached MaxTiles=%d, stop."), MaxTiles);
                    goto DONE;
                }

                // --- World tile box ---
                const FVector BoxMinWS(x, y, z);
                const FVector BoxMaxWS(
                    FMath::Min(x + Tile, BoundsWS.Max.X),
                    FMath::Min(y + Tile, BoundsWS.Max.Y),
                    FMath::Min(z + Tile, BoundsWS.Max.Z)
                );
                const FBox BoxWS(BoxMinWS, BoxMaxWS);

                // --- World -> ComponentLocal ---
                FVector LocalMin = CloudTfInv.TransformPosition(BoxWS.Min);
                FVector LocalMax = CloudTfInv.TransformPosition(BoxWS.Max);

                // --- ComponentLocal -> CloudCenteredLocal（减 LocationOffset）---
                LocalMin -= CloudOffset;
                LocalMax -= CloudOffset;

                // 防止 min/max 交换：用 AABB 重建
                const FVector LocalCenter = (LocalMin + LocalMax) * 0.5f;
                const FVector LocalExtent = (LocalMin - LocalMax).GetAbs() * 0.5f;
                const FBox LocalBox = FBox::BuildAABB(LocalCenter, LocalExtent);

                TilePoints.Reset();

                Cloud->GetPointsInBoxAsCopies(
                    TilePoints,
                    LocalBox,
                    /*bVisibleOnly=*/false,
                    /*bReturnWorldSpace=*/false
                );

                const int64 N = TilePoints.Num();
                if (N <= 0) continue;

                const int64 Step = (N > MaxPointsPerTile) ? (N / MaxPointsPerTile + 1) : 1;

                if (!bLoggedFirstPoint)
                {
                    const FVector P0Centered = ExtractPointWorldLocation(TilePoints[0]);   // centered local
                    const FVector P0Local = P0Centered + CloudOffset;                      // component local
                    const FVector P0WS = CloudTf.TransformPosition(P0Local);               // world

                    UE_LOG(LogTemp, Warning, TEXT("[Align] FirstPoint Centered=%s  +Offset=%s  -> Local=%s  -> WS=%s"),
                        *P0Centered.ToString(), *CloudOffset.ToString(), *P0Local.ToString(), *P0WS.ToString());

                    bLoggedFirstPoint = true;
                }

                for (int64 i = 0; i < N; i += Step)
                {
                    const FLidarPointCloudPoint& P = TilePoints[i];

                    const FVector PosCentered = ExtractPointWorldLocation(P); // centered local
                    const FVector PosLocal = PosCentered + CloudOffset;       // component local
                    const FVector PosWS = CloudTf.TransformPosition(PosLocal);// world

                    const int32 ix = FMath::FloorToInt((PosWS.X - OriginWS.X) / VoxelSize);
                    const int32 iy = FMath::FloorToInt((PosWS.Y - OriginWS.Y) / VoxelSize);
                    const int32 iz = FMath::FloorToInt((PosWS.Z - OriginWS.Z) / VoxelSize);

                    Occupied.Add(FIntVector(ix, iy, iz));

                    if (Occupied.Num() >= MaxTotalVoxels * 2)
                        goto DONE;
                }

                TotalPointsUsed += (N / Step);
            }
        }
    }

DONE:
    UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Tiles=%d  TotalPointsUsed~=%lld  Occupied(pre)=%d"),
        TilesVisited, (long long)TotalPointsUsed, Occupied.Num());

    RenderFromOccupiedVoxels(Occupied, OriginWS);
}

void AVoxelGeofenceActor::RenderFromOccupiedVoxels(const TSet<FIntVector>& Occupied, const FVector& OriginWS)
{
    InstancedMesh->ClearInstances();

    if (Occupied.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("VoxelGeofenceActor: Occupied is empty."));
        return;
    }

    static const FIntVector N6[6] = {
        FIntVector(1, 0, 0),
        FIntVector(-1, 0, 0),
        FIntVector(0, 1, 0),
        FIntVector(0,-1, 0),
        FIntVector(0, 0, 1),
        FIntVector(0, 0,-1),
    };

    const FVector Scale(VoxelSize / 100.f);
    int32 Added = 0;

    for (const FIntVector& V : Occupied)
    {
        bool bDraw = true;

        if (bShowOnlyBoundary)
        {
            bDraw = false;
            for (int i = 0; i < 6; ++i)
            {
                if (!Occupied.Contains(V + N6[i]))
                {
                    bDraw = true;
                    break;
                }
            }
        }

        if (!bDraw) continue;

        const FVector CenterWS = OriginWS + FVector(
            (V.X + 0.5f) * VoxelSize,
            (V.Y + 0.5f) * VoxelSize,
            (V.Z + 0.5f) * VoxelSize
        );

        InstancedMesh->AddInstance(FTransform(FRotator::ZeroRotator, CenterWS, Scale));

        if (++Added >= MaxTotalVoxels)
            break;
    }

    UE_LOG(LogTemp, Warning, TEXT("[VoxelFence] Rendered=%d (BoundaryOnly=%s)"),
        Added, bShowOnlyBoundary ? TEXT("true") : TEXT("false"));
}
