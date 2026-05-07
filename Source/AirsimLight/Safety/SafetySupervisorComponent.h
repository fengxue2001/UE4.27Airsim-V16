#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SafetySupervisorComponent.generated.h"

class AGeoFenceVolume;
class AFlyingPawn;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AIRSIMLIGHT_API USafetySupervisorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USafetySupervisorComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    void UpdateSafety();

    /** —— 运行时状态 —— */
    int32   CurrentLevel = 0;                // 0=安全,1=预警,2=危险,3=越界
    FVector PrevPos = FVector::ZeroVector;
    bool    bHasPrevPos = false;
    float   SmoothedSpeed = 0.f;             // cm/s（EMA 平滑后）

    /** —— 引用对象 —— */
    UPROPERTY() AGeoFenceVolume* FenceRef = nullptr;
    UPROPERTY() UStaticMeshComponent* ShieldMesh = nullptr;
    UPROPERTY() UMaterialInstanceDynamic* ShieldMaterial = nullptr;

public:

    /** 基础护盾半径（cm） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety")
    float ProtectionRadius = 80.f;

    /** 此参数你目前没有使用，但先保留 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety")
    float ShieldScaleFactor = 1.2f;

    /** 分级距离（cm） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety")
    float L1Distance = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety")
    float L2Distance = 30.f;

    /** 速度平滑时间常数 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety|Speed")
    float SpeedEMA_Tau = 0.25f;

    /** 速度映射上限（cm/s） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety|Visual")
    float MaxSpeedForShield = 500.f;

    /** 速度引起护盾拉伸的三轴增益 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety|Visual")
    float SpeedGainX = 0.60f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety|Visual")
    float SpeedGainY = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safety|Visual")
    float SpeedGainZ = 0.15f;
};
