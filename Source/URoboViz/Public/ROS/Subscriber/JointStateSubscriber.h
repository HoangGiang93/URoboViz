// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSSubscriber.h"
// clang-format off
#include "JointStateSubscriber.generated.h"
// clang-format on

class UJointController;

UCLASS()
class UROBOVIZ_API UJointStateSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UJointStateSubscriber();

protected:
  void CreateSubscriber() override;

public:
  UPROPERTY(EditAnywhere)
  FString JointControllerName = TEXT("JointController");
};

class UROBOVIZ_API FJointStateSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FJointStateSubscriberCallback(FString InTopic, FString InType, UJointController *InJointController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UJointController *JointController;
};