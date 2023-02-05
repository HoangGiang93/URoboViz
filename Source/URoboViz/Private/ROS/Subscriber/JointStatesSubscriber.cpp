// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/JointStatesSubscriber.h"
#include "Controllers/JointController.h"
#include "sensor_msgs/JointState.h"

DEFINE_LOG_CATEGORY_STATIC(LogJointStatesSubscriber, Log, All)

UJointStatesSubscriber::UJointStatesSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/joint_states");
  CommonSubscriberParameters.MessageType = TEXT("sensor_msgs/JointState");
}

void UJointStatesSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogJointStatesSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  if (UJointController *JointController = Cast<UJointController>(GetRoboManager()->GetController(JointControllerName)))
  {
    Subscriber = MakeShareable<FJointStatesSubscriberCallback>(
        new FJointStatesSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, JointController));
  }
  else
  {
    UE_LOG(LogJointStatesSubscriber, Warning, TEXT("%s of %s not found"), *JointControllerName, *GetName())
  }
}

FJointStatesSubscriberCallback::FJointStatesSubscriberCallback(
    FString InTopic, FString InType, UJointController *InJointController) : FROSBridgeSubscriber(InTopic, InType)
{
  JointController = InJointController;
}

TSharedPtr<FROSBridgeMsg> FJointStatesSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<sensor_msgs::JointState> JointState =
      MakeShareable<sensor_msgs::JointState>(new sensor_msgs::JointState());

  JointState->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(JointState);
}

void FJointStatesSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (JointController)
  {
    TSharedPtr<sensor_msgs::JointState> JointState = StaticCastSharedPtr<sensor_msgs::JointState>(Msg);
    for (int32 i = 0; i < JointState->Names.Num(); i++)
    {
      JointController->SetDesiredJointPositionFromROS(JointState->GetName()[i], JointState->GetPosition()[i]);
    }
  }
}