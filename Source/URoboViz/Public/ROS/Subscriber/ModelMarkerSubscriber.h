// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSSubscriber.h"
#include "ModelMarkerSubscriber.generated.h"

class ObjectController;

UCLASS()
class UROBOVIZ_API UModelMarkerSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UModelMarkerSubscriber();

protected:
  void CreateSubscriber() override;

};

class UROBOVIZ_API FModelMarkerSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FModelMarkerSubscriberCallback(FString InTopic, FString InType, UObjectController *InObjectController);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UObjectController *ObjectController;
};