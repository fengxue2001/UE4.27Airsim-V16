#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeoFenceVolume.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class AIRSIMLIGHT_API AGeoFenceVolume : public AActor
{
    GENERATED_BODY()

public:
    AGeoFenceVolume();

protected:
    virtual void BeginPlay() override;

public:

    /** 主 Box 组件（围栏体积） */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Geofence")
    UBoxComponent* BoxComp;

    /** 点是否在围栏内（含边界） */
    UFUNCTION(BlueprintCallable, Category = "Geofence")
    bool IsInside(const FVector& Point) const;

    /** 点到围栏边界的最小距离（<0 表示在围栏外） */
    UFUNCTION(BlueprintCallable, Category = "Geofence")
    float DistanceToBoundary(const FVector& Point) const;
};
