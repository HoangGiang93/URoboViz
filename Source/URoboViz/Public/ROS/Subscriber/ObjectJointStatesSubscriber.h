// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSSubscriber.h"
// clang-format off
#include "ObjectJointStatesSubscriber.generated.h"
// clang-format on

class ObjectController;

UCLASS()
class UROBOVIZ_API UObjectJointStatesSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UObjectJointStatesSubscriber();

protected:
  void CreateSubscriber() override;

};

class UROBOVIZ_API FObjectJointStatesSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FObjectJointStatesSubscriberCallback(FString InTopic, FString InType, UObjectController *InObjectController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UObjectController *ObjectController;
};