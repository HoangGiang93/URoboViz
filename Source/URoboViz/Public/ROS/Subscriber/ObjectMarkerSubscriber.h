// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSSubscriber.h"
// clang-format off
#include "ObjectMarkerSubscriber.generated.h"
// clang-format on

class ObjectController;

UCLASS()
class UROBOVIZ_API UObjectMarkerSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UObjectMarkerSubscriber();

protected:
  void CreateSubscriber() override;
};

class UROBOVIZ_API FObjectMarkerSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FObjectMarkerSubscriberCallback(FString InTopic, FString InType, UObjectController *InObjectController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UObjectController *ObjectController;
};