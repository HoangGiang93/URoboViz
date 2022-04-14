// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSPublisher.h"

#include "LidarPublisher.generated.h"

UCLASS()
class UROBOVIZ_API ULidarPublisher : public UROSPublisher
{
  GENERATED_BODY()

public:
  ULidarPublisher();

protected:
  void Init() override;
  
  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FString LidarName = TEXT("Lidar");

  UPROPERTY(EditAnywhere)
  FString FrameId = TEXT("");

private:
  class ULidar* Lidar;

  int32 Seq = 0;
};