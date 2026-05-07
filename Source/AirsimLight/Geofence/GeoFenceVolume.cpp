#include "Airsimlight/Geofence/GeoFenceVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

AGeoFenceVolume::AGeoFenceVolume()
{
    PrimaryActorTick.bCanEverTick = false;

    // 创建 Box 组件
    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    SetRootComponent(BoxComp);

    // 默认 20m × 20m × 10m（单位 cm）
    BoxComp->SetBoxExtent(FVector(1000.f, 1000.f, 500.f));

    // 设置为触发器，不阻挡
    BoxComp->SetCollisionProfileName(TEXT("Trigger"));
    BoxComp->SetGenerateOverlapEvents(true);

    // 可视化设置（编辑器与游戏均显示）
    BoxComp->SetHiddenInGame(false);
    BoxComp->SetVisibility(true);

#if WITH_EDITORONLY_DATA
    BoxComp->ShapeColor = FColor(0, 255, 0, 128); // 半透明绿色
#endif
}

void AGeoFenceVolume::BeginPlay()
{
    Super::BeginPlay();

    if (BoxComp)
    {
        FVector Ext = BoxComp->GetScaledBoxExtent();
        FVector Center = BoxComp->GetComponentLocation();
        UE_LOG(LogTemp, Warning, TEXT("GeoFenceVolume ready at %s, Extent %s"),
            *Center.ToString(), *Ext.ToString());
    }
}

bool AGeoFenceVolume::IsInside(const FVector& Point) const
{
    if (!BoxComp) return false;

    const FTransform& T = BoxComp->GetComponentTransform();
    const FVector Local = T.InverseTransformPosition(Point);
    const FVector Ext = BoxComp->GetUnscaledBoxExtent();

    return FMath::Abs(Local.X) <= Ext.X &&
        FMath::Abs(Local.Y) <= Ext.Y &&
        FMath::Abs(Local.Z) <= Ext.Z;
}

float AGeoFenceVolume::DistanceToBoundary(const FVector& Point) const
{
    if (!BoxComp) return -FLT_MAX;

    const FTransform& T = BoxComp->GetComponentTransform();
    const FVector Local = T.InverseTransformPosition(Point);
    const FVector Ext = BoxComp->GetUnscaledBoxExtent();

    float dx = Ext.X - FMath::Abs(Local.X);
    float dy = Ext.Y - FMath::Abs(Local.Y);
    float dz = Ext.Z - FMath::Abs(Local.Z);

    return FMath::Min3(dx, dy, dz); // <0 表示越界
}
