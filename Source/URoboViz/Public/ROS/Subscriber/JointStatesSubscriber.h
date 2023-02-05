// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSSubscriber.h"
// clang-format off
#include "JointStatesSubscriber.generated.h"
// clang-format on

class UJointController;

UCLASS()
class UROBOVIZ_API UJointStatesSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UJointStatesSubscriber();

protected:
  void CreateSubscriber() override;

public:
  UPROPERTY(EditAnywhere)
  FString JointControllerName = TEXT("JointController");
};

class UROBOVIZ_API FJointStatesSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FJointStatesSubscriberCallback(FString InTopic, FString InType, UJointController *InJointController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UJointController *JointController;
};