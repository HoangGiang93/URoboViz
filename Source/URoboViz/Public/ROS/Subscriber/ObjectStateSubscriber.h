// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROSSubscriber.h"
// clang-format off
#include "ObjectStateSubscriber.generated.h"
// clang-format on

class ObjectController;

UCLASS()
class UROBOVIZ_API UObjectStateSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UObjectStateSubscriber();

protected:
  void CreateSubscriber() override;
};

class UROBOVIZ_API FObjectStateSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FObjectStateSubscriberCallback(FString InTopic, FString InType, UObjectController *InObjectController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UObjectController *ObjectController;
};