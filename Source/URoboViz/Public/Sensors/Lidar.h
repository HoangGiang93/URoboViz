// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "Sensor.h"
// clang-format off
#include "Lidar.generated.h"
// clang-format on

class ASkeletalMeshActor;

UCLASS()
class UROBOVIZ_API ULidar : public USensor
{
	GENERATED_BODY()

public:
	ULidar();

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void Init() override;

public:
  FORCEINLINE TArray<float> GetRanges() const { return Ranges; }

  FORCEINLINE float GetScanTime() const { return TimeIncrement * 2 * PI / AngleIncrement; }

public:
  UPROPERTY(EditAnywhere)
  bool bDrawLines = false;

  UPROPERTY(EditAnywhere)
  bool bDrawPoints = true;

  UPROPERTY(EditAnywhere)
  FName BoneName = TEXT("root_bone");

  UPROPERTY(EditAnywhere)
  float AngleMin = 0.f;

  UPROPERTY(EditAnywhere)
  float AngleMax = 2 * PI;

  UPROPERTY(EditAnywhere)
  float AngleIncrement = PI / 180.f;

  UPROPERTY(EditAnywhere)
  float TimeIncrement = 0.f;

  UPROPERTY(EditAnywhere)
  float RangeMin = 0.4f;

  UPROPERTY(EditAnywhere)
  float RangeMax = 100.f;

private:
  UPROPERTY(VisibleAnywhere)
  TArray<float> Ranges;

  float ScanTimeStart;

  float AngleStart;

  float AngleEnd;

  FCollisionQueryParams TraceParams;

  FCollisionObjectQueryParams ObjectParams;

  FTransform LastTransform;
};
