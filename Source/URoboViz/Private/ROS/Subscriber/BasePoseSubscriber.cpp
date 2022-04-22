// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/BasePoseSubscriber.h"
#include "Controllers/BaseController.h"
#include "nav_msgs/Odometry.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePoseSubscriber, Log, All)

UBasePoseSubscriber::UBasePoseSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/base_footprint");
  CommonSubscriberParameters.MessageType = TEXT("nav_msgs/Odometry");
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
  TSharedPtr<nav_msgs::Odometry> BasePose =
      MakeShareable<nav_msgs::Odometry>(new nav_msgs::Odometry());

  BasePose->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(BasePose);
}

void FBasePoseSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (BaseController != nullptr)
  {
    TSharedPtr<nav_msgs::Odometry> BasePose = StaticCastSharedPtr<nav_msgs::Odometry>(Msg);

    FVector BaseLocation = BasePose->GetPose().GetPose().GetPosition().GetVector();
    FQuat BaseRotation = BasePose->GetPose().GetPose().GetOrientation().GetQuat();

    BaseController->SetDesiredBasePoseFromROS(BaseLocation, BaseRotation);
  }
}