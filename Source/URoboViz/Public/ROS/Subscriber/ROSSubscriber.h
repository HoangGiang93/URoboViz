// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ROS/ROSBase.h"

#include "ROSSubscriber.generated.h"

USTRUCT()
struct FRSubscriberParameter
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

public:
  virtual void Init() override;

protected:
  virtual void CreateSubscriber() {}

public:
  UPROPERTY(EditAnywhere)
  FRSubscriberParameter CommonSubscriberParameters;

protected:
  TSharedPtr<FROSBridgeSubscriber> Subscriber;
};