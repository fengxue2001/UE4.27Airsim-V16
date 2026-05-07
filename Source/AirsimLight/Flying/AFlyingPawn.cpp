//#include "FlyingPawn.h"
//#include "Components/StaticMeshComponent.h"
//#include "AirBlueprintLib.h"
//#include "common/CommonStructs.hpp"
//#include "common/Common.hpp"
//
//AFlyingPawn::AFlyingPawn()
//{
//    // 旋翼可视化的初始化与更新
//    init_id_ = pawn_events_.getActuatorSignal().connect_member(this, &AFlyingPawn::initializeRotors);
//    pawn_events_.getActuatorSignal().connect_member(this, &AFlyingPawn::setRotorSpeed);
//
//    // 你的 AirSim 版本无 RC 钩子：不连接 getRCDataSignal()
//}
//
//void AFlyingPawn::BeginPlay()
//{
//    Super::BeginPlay();
//}
//
//void AFlyingPawn::initializeForBeginPlay()
//{
//    camera_front_right_ = Cast<APIPCamera>(
//        (UAirBlueprintLib::GetActorComponent<UChildActorComponent>(this, TEXT("FrontRightCamera")))->GetChildActor());
//    camera_front_left_ = Cast<APIPCamera>(
//        (UAirBlueprintLib::GetActorComponent<UChildActorComponent>(this, TEXT("FrontLeftCamera")))->GetChildActor());
//    camera_front_center_ = Cast<APIPCamera>(
//        (UAirBlueprintLib::GetActorComponent<UChildActorComponent>(this, TEXT("FrontCenterCamera")))->GetChildActor());
//    camera_back_center_ = Cast<APIPCamera>(
//        (UAirBlueprintLib::GetActorComponent<UChildActorComponent>(this, TEXT("BackCenterCamera")))->GetChildActor());
//    camera_bottom_center_ = Cast<APIPCamera>(
//        (UAirBlueprintLib::GetActorComponent<UChildActorComponent>(this, TEXT("BottomCenterCamera")))->GetChildActor());
//}
//
//void AFlyingPawn::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//    pawn_events_.getPawnTickSignal().emit(DeltaSeconds);
//}
//
//void AFlyingPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//    camera_front_right_ = nullptr;
//    camera_front_left_ = nullptr;
//    camera_front_center_ = nullptr;
//    camera_back_center_ = nullptr;
//    camera_bottom_center_ = nullptr;
//
//    pawn_events_.getActuatorSignal().disconnect_all();
//    rotating_movements_.Empty();
//
//    Super::EndPlay(EndPlayReason);
//}
//
//const common_utils::UniqueValueMap<std::string, APIPCamera*> AFlyingPawn::getCameras() const
//{
//    common_utils::UniqueValueMap<std::string, APIPCamera*> cameras;
//    cameras.insert_or_assign("front_center", camera_front_center_);
//    cameras.insert_or_assign("front_right", camera_front_right_);
//    cameras.insert_or_assign("front_left", camera_front_left_);
//    cameras.insert_or_assign("bottom_center", camera_bottom_center_);
//    cameras.insert_or_assign("back_center", camera_back_center_);
//
//    cameras.insert_or_assign("0", camera_front_center_);
//    cameras.insert_or_assign("1", camera_front_right_);
//    cameras.insert_or_assign("2", camera_front_left_);
//    cameras.insert_or_assign("3", camera_bottom_center_);
//    cameras.insert_or_assign("4", camera_back_center_);
//
//    cameras.insert_or_assign("", camera_front_center_);
//    cameras.insert_or_assign("fpv", camera_front_center_);
//
//    return cameras;
//}
//
//void AFlyingPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp,
//    bool bSelfMoved, FVector HitLocation, FVector HitNormal,
//    FVector NormalImpulse, const FHitResult& Hit)
//{
//    pawn_events_.getCollisionSignal().emit(MyComp, Other, OtherComp, bSelfMoved,
//        HitLocation, HitNormal, NormalImpulse, Hit);
//}
//
///** 初始化旋翼组件 */
//void AFlyingPawn::initializeRotors(const std::vector<MultirotorPawnEvents::RotorActuatorInfo>& rotor_infos)
//{
//    for (int i = 0; i < static_cast<int>(rotor_infos.size()); ++i) {
//        rotating_movements_.Add(UAirBlueprintLib::GetActorComponent<URotatingMovementComponent>(
//            this, TEXT("Rotation") + FString::FromInt(i)));
//    }
//    pawn_events_.getActuatorSignal().disconnect(init_id_);
//}
//
///** 每帧由 AirSim 调用：这里乘上推力缩放（可视化一致） */
//void AFlyingPawn::setRotorSpeed(const std::vector<MultirotorPawnEvents::RotorActuatorInfo>& rotor_infos)
//{
//    const float scale = bEmergencyStop_ ? 0.0f : FMath::Clamp(ThrustScale_, 0.0f, 1.0f);
//
//    for (int rotor_index = 0; rotor_index < static_cast<int>(rotor_infos.size()); ++rotor_index) {
//        if (!rotating_movements_.IsValidIndex(rotor_index)) continue;
//        if (auto* comp = rotating_movements_[rotor_index]) {
//            const float omega = rotor_infos[rotor_index].rotor_speed *
//                rotor_infos[rotor_index].rotor_direction *
//                scale; // ? 乘以限推
//            comp->RotationRate.Yaw = omega * 180.0f / M_PIf * RotatorFactor;
//        }
//    }
//}
//
///** 设置推力限幅（0..1） */
//void AFlyingPawn::ApplySpeedLimit(float Factor)
//{
//    ThrustScale_ = FMath::Clamp(Factor, 0.0f, 1.0f);
//    bEmergencyStop_ = false; // 解除急停
//    UE_LOG(LogTemp, Warning, TEXT("ApplySpeedLimit: ThrustScale_ = %.2f"), ThrustScale_);
//}
//
///** 急停：直接 0 推力 */
//void AFlyingPawn::EmergencyStop()
//{
//    bEmergencyStop_ = true;
//    ThrustScale_ = 0.0f;
//    UE_LOG(LogTemp, Warning, TEXT("EmergencyStop TRIGGERED: thrust -> 0"));
//}
