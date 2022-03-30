// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSSubscriber.h"
#include "JointStateSubscriber.generated.h"

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
  FString JointControllerName;
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