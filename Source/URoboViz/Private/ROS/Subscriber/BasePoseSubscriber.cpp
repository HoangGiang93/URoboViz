// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/BasePoseSubscriber.h"
#include "Controllers/BaseController.h"
#include "geometry_msgs/TransformStamped.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePoseSubscriber, Log, All)

UBasePoseSubscriber::UBasePoseSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/base_footprint");
  CommonSubscriberParameters.MessageType = TEXT("geometry_msgs/TransformStamped");
}

void UBasePoseSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogBasePoseSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  if (UBaseController *BaseController = Cast<UBaseController>(GetRoboManager()->GetController(BaseControllerName)))
  {
    Subscriber = MakeShareable<FBasePoseSubscriberCallback>(
        new FBasePoseSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, BaseController));
  }
  else
  {
    UE_LOG(LogBasePoseSubscriber, Warning, TEXT("%s of %s not found"), *BaseControllerName, *GetName())
  }
}

FBasePoseSubscriberCallback::FBasePoseSubscriberCallback(
    FString InTopic, FString InType, UBaseController *InBaseController) : FROSBridgeSubscriber(InTopic, InType)
{
  BaseController = InBaseController;
}

TSharedPtr<FROSBridgeMsg> FBasePoseSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<geometry_msgs::TransformStamped> BasePose =
      MakeShareable<geometry_msgs::TransformStamped>(new geometry_msgs::TransformStamped());

  BasePose->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(BasePose);
}

void FBasePoseSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (BaseController != nullptr)
  {
    TSharedPtr<geometry_msgs::TransformStamped> BasePose = StaticCastSharedPtr<geometry_msgs::TransformStamped>(Msg);

    FVector BaseLocation = BasePose->GetTransform().GetTranslation().GetVector();
    FQuat BaseRotation = BasePose->GetTransform().GetRotation().GetQuat();

    BaseController->SetDesiredBasePoseFromROS(BaseLocation, BaseRotation);
  }
}