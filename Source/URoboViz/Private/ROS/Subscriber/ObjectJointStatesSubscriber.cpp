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
    FString FrameId = ObjectJointStates->GetHeader().GetFrameId();

    if (!JointNameMap.Contains(FrameId))
    {
      JointNameMap.Add(FrameId);
    }

    for (int32 i = 0; i < ObjectJointStates->GetName().Num(); i++)
    {
      FString ObjectJointName;
      if (!JointNameMap[FrameId].Contains(ObjectJointStates->GetName()[i]))
      {
        ObjectJointName = ObjectJointStates->GetName()[i];
        int32 LastIndex;
        if (ObjectJointName.FindLastChar(*TEXT("_"), LastIndex))
        {
          ObjectJointName = ObjectJointName.Left(LastIndex);
        }
        JointNameMap[FrameId].Add(ObjectJointStates->GetName()[i], ObjectJointName);
      }

      ObjectJointName = JointNameMap[FrameId][ObjectJointStates->GetName()[i]];
      const float DesiredObjectJointPosition = ObjectJointStates->GetPosition()[i];
      ObjectController->SetDesiredObjectJointPositionFromROS(FrameId, ObjectJointName, DesiredObjectJointPosition);
    }
  }
}