// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROSSubscriber.h"
// clang-format off
#include "BasePoseSubscriber.generated.h"
// clang-format on

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
  FString BaseControllerName = TEXT("BaseController");

  UPROPERTY(EditAnywhere)
  FString BaseName = TEXT("base_footprint");
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