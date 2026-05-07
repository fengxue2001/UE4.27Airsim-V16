//#pragma once
//
//#include "GameFramework/RotatingMovementComponent.h"
//
//#include <memory>
//#include "PIPCamera.h"
//#include "common/common_utils/Signal.hpp"
//#include "common/common_utils/UniqueValueMap.hpp"
//#include "MultirotorPawnEvents.h"
//
//#include "FlyingPawn.generated.h"
//
//UCLASS()
//class AIRSIM_API AFlyingPawn : public APawn
//{
//    GENERATED_BODY()
//
//public:
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
//    float RotatorFactor = 1.0f;
//
//    AFlyingPawn();
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaSeconds) override;
//    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//    virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp,
//        bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
//
//    // 摄像头/旋翼初始化 & 控制
//    void initializeForBeginPlay();
//    void initializeRotors(const std::vector<MultirotorPawnEvents::RotorActuatorInfo>& rotor_infos);
//    void setRotorSpeed(const std::vector<MultirotorPawnEvents::RotorActuatorInfo>& rotor_infos);
//
//    // 摄像头集合
//    const common_utils::UniqueValueMap<std::string, APIPCamera*> getCameras() const;
//
//    MultirotorPawnEvents* getPawnEvents() { return &pawn_events_; }
//
//    /** ===== 安全控制接口（与你现有调用保持一致） ===== */
//    /** 限速：0.0 = 停止, 1.0 = 全速（作用于 setRotorSpeed 的缩放） */
//    UFUNCTION(BlueprintCallable, Category = "Safety")
//    void ApplySpeedLimit(float Factor);
//
//    /** 急停：立刻 0 推力 */
//    UFUNCTION(BlueprintCallable, Category = "Safety")
//    void EmergencyStop();
//
//private:
//    // 摄像头
//    UPROPERTY() APIPCamera* camera_front_left_;
//    UPROPERTY() APIPCamera* camera_front_right_;
//    UPROPERTY() APIPCamera* camera_front_center_;
//    UPROPERTY() APIPCamera* camera_back_center_;
//    UPROPERTY() APIPCamera* camera_bottom_center_;
//
//    // 旋翼
//    UPROPERTY() TArray<URotatingMovementComponent*> rotating_movements_;
//
//    MultirotorPawnEvents pawn_events_;
//    int init_id_ = -1;
//
//    /** 推力缩放（限速核心），默认 1.0 */
//    float ThrustScale_ = 1.0f;
//
//    /** 急停开关；为 true 时推力 0 */
//    bool bEmergencyStop_ = false;
//};
