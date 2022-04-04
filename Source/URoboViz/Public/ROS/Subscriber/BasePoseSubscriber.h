// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSSubscriber.h"
#include "BasePoseSubscriber.generated.h"

class UBaseController;

UCLASS()
class UROBOVIZ_API UBasePoseSubscriber final : public UROSSubscriber
{
  GENERATED_BODY()

public:
  UBasePoseSubscriber();

protected:
  void CreateSubscriber() override;

public:
  UPROPERTY(EditAnywhere)
  FString BaseControllerName;

  UPROPERTY(EditAnywhere)
  FString BaseName;
};

class UROBOVIZ_API FBasePoseSubscriberCallback final : public FROSBridgeSubscriber
{
public:
  FBasePoseSubscriberCallback(FString InTopic, FString InType, UBaseController *InBaseController, FString InBaseName);

  TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

  void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:
  UBaseController *BaseController;

  FString BaseName;
};