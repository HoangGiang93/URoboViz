// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROS/ROSBase.h"
// clang-format off
#include "ROSSubscriber.generated.h"
// clang-format on

USTRUCT()
struct FROSSubscriberParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString Topic;

  UPROPERTY(VisibleAnywhere)
  FString MessageType;
};

UCLASS()
class UROBOVIZ_API UROSSubscriber : public UROSBase
{
  GENERATED_BODY()

protected:
  virtual void Init() override;

protected:
  virtual void CreateSubscriber() {}

public:
  UPROPERTY(EditAnywhere)
  FROSSubscriberParameter CommonSubscriberParameters;

protected:
  TSharedPtr<FROSBridgeSubscriber> Subscriber;
};