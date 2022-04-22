// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROSSubscriber.h"
// clang-format off
#include "ObjectStateArraySubscriber.generated.h"
// clang-format on

class ObjectController;

UCLASS()
class UROBOVIZ_API UObjectStateArraySubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UObjectStateArraySubscriber();

protected:
  void CreateSubscriber() override;
};

class UROBOVIZ_API FObjectStateArraySubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FObjectStateArraySubscriberCallback(FString InTopic, FString InType, UObjectController *InObjectController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UObjectController *ObjectController;
};