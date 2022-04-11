// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSSubscriber.h"
#include "ObjectMarkerSubscriber.generated.h"

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