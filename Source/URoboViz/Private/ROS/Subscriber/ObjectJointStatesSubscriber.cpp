// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/ObjectJointStatesSubscriber.h"
#include "ObjectController.h"
#include "sensor_msgs/JointState.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectJointStatesSubscriber, Log, All)

UObjectJointStatesSubscriber::UObjectJointStatesSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/mujoco/object_joint_states");
  CommonSubscriberParameters.MessageType = TEXT("sensor_msgs/JointState");
}

void UObjectJointStatesSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogObjectJointStatesSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  if (UObjectController *ObjectController = Cast<UObjectController>(GetRoboManager()->GetObjectController()))
  {
    Subscriber = MakeShareable<FObjectJointStatesSubscriberCallback>(
        new FObjectJointStatesSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, ObjectController));
  }
}

FObjectJointStatesSubscriberCallback::FObjectJointStatesSubscriberCallback(
    FString InTopic, FString InType, UObjectController *InObjectController) : FROSBridgeSubscriber(InTopic, InType)
{
  ObjectController = InObjectController;
}

TSharedPtr<FROSBridgeMsg> FObjectJointStatesSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<sensor_msgs::JointState> ObjectJointStates =
      MakeShareable<sensor_msgs::JointState>(new sensor_msgs::JointState());

  ObjectJointStates->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(ObjectJointStates);
}

void FObjectJointStatesSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (ObjectController)
  {
    TSharedPtr<sensor_msgs::JointState> ObjectJointStates = StaticCastSharedPtr<sensor_msgs::JointState>(Msg);
    for (int32 i = 0; i < ObjectJointStates->Names.Num(); i++)
    {
      ObjectController->SetDesiredObjectJointPositionFromROS(ObjectJointStates->GetName()[i], ObjectJointStates->GetPosition()[i]);
    }
  }
}