// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROS/ROSBase.h"

#include "ROSPublisher.generated.h"

USTRUCT()
struct FROSPublisherParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  float PublishRate = 60.f;

  UPROPERTY(EditAnywhere)
  FString Topic;

  UPROPERTY(VisibleAnywhere)
  FString MessageType;
};

UCLASS()
class UROBOVIZ_API UROSPublisher : public UROSBase
{
  GENERATED_BODY()

public:
  void Tick();

protected:
  virtual void Init() override;

protected:
  virtual void Publish() {}

public:
  UPROPERTY(EditAnywhere)
  FROSPublisherParameter CommonPublisherParameters;

protected:
  TSharedPtr<FROSBridgePublisher> Publisher;

private:
  FROSTime PublishTime;
};