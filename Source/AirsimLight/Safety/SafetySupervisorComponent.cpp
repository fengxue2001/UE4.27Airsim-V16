#include "Airsimlight/Safety/SafetySupervisorComponent.h"
#include "Airsimlight/Geofence/GeoFenceVolume.h"
// FlyingPawn 仍在插件目录，路径不变
#include "Vehicles/Multirotor/FlyingPawn.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"


USafetySupervisorComponent::USafetySupervisorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    CurrentLevel = 0;
    FenceRef = nullptr;
    ShieldMesh = nullptr;
    ShieldMaterial = nullptr;
    PrevPos = FVector::ZeroVector;
    bHasPrevPos = false;
    SmoothedSpeed = 0.f;
}

void USafetySupervisorComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (Owner)
    {
        // 查找名为 ShieldMesh 的静态网格体
        TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());
        for (UActorComponent* Comp : Components)
        {
            if (Comp && Comp->GetName() == TEXT("ShieldMesh"))
            {
                ShieldMesh = Cast<UStaticMeshComponent>(Comp);
                break;
            }
        }

        if (ShieldMesh && ShieldMesh->GetMaterial(0))
        {
            ShieldMaterial = ShieldMesh->CreateAndSetMaterialInstanceDynamic(0);
            UE_LOG(LogTemp, Log, TEXT("SafetySupervisor: Dynamic material set on ShieldMesh."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SafetySupervisor: ShieldMesh missing or no material (need Shield_Mat)."));
        }
    }

    // 找到场景中的第一个 GeoFenceVolume
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGeoFenceVolume::StaticClass(), Found);
    if (Found.Num() > 0)
        FenceRef = Cast<AGeoFenceVolume>(Found[0]);
}

void USafetySupervisorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateSafety();
}

void USafetySupervisorComponent::UpdateSafety()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    AFlyingPawn* Drone = Cast<AFlyingPawn>(Owner);
    if (!Drone) return;

    // === 1) 平滑速度计算 ===
    const float dt = GetWorld()->GetDeltaSeconds();
    const FVector CurPos = Owner->GetActorLocation();

    float InstSpeed = 0.f;
    if (bHasPrevPos && dt > KINDA_SMALL_NUMBER)
        InstSpeed = (CurPos - PrevPos).Size() / dt;

    PrevPos = CurPos;
    bHasPrevPos = true;

    const float alpha = 1.f - FMath::Exp(-dt / FMath::Max(0.001f, SpeedEMA_Tau));
    SmoothedSpeed += alpha * (InstSpeed - SmoothedSpeed);

    // === 2) 根据围栏距离决定等级 ===
    int32 NewLevel = 0;
    if (FenceRef)
    {
        const float Dist = FenceRef->DistanceToBoundary(CurPos);

        if (Dist < 0.f)             NewLevel = 3;
        else if (Dist < L2Distance) NewLevel = 2;
        else if (Dist < L1Distance) NewLevel = 1;
        else                        NewLevel = 0;
    }

    CurrentLevel = NewLevel;

    // === 3) 调整无人机速度限制 ===
    if (CurrentLevel == 0)      Drone->ApplySpeedLimit(1.0f);
    else if (CurrentLevel == 1) Drone->ApplySpeedLimit(0.6f);
    else if (CurrentLevel == 2) Drone->ApplySpeedLimit(0.3f);
    else if (CurrentLevel == 3) Drone->EmergencyStop();

    // === 4) 碰撞盾（大小：随速度各向异性缩放，无呼吸） ===
    if (ShieldMesh)
    {
        // 0..1 速度归一化 & 非线性映射
        const float v01 = FMath::Clamp(SmoothedSpeed / FMath::Max(1.f, MaxSpeedForShield), 0.f, 1.f);
        const float vCurve = FMath::Pow(v01, 1.5f);

        // 基础半径（cm -> UE scale）
        const float BaseScale = ProtectionRadius / 50.0f;

        // 三轴随速度拉伸
        const float ScaleX = BaseScale * (1.0f + SpeedGainX * vCurve);
        const float ScaleY = BaseScale * (1.0f + SpeedGainY * vCurve);
        const float ScaleZ = BaseScale * (1.0f + SpeedGainZ * vCurve);

        ShieldMesh->SetWorldScale3D(FVector(ScaleX, ScaleY, ScaleZ));
        ShieldMesh->SetWorldLocation(Owner->GetActorLocation());
    }

    // === 5) 护盾材质颜色变化（保留） ===
    if (ShieldMaterial)
    {
        FLinearColor NewColor =
            (CurrentLevel == 0) ? FLinearColor::Green :
            (CurrentLevel == 1) ? FLinearColor::Yellow :
            (CurrentLevel == 2) ? FLinearColor(1.f, 0.5f, 0.f) :
            FLinearColor::Red;

        ShieldMaterial->SetVectorParameterValue(TEXT("ShieldColor"), NewColor);

        float Opacity = 0.25f;
        if (CurrentLevel >= 2)
            Opacity = 0.5f + 0.5f * FMath::Sin(GetWorld()->TimeSeconds * 10.0f);

        ShieldMaterial->SetScalarParameterValue(TEXT("OpacityValue"), Opacity);
    }
}
