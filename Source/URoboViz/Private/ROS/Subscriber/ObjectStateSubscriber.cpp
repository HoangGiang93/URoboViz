// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/ObjectStateSubscriber.h"
#include "Engine/StaticMeshActor.h"
#include "ObjectController.h"
#include "mujoco_msgs/ObjectState.h"
#include "visualization_msgs/Marker.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectStateSubscriber, Log, All)

UObjectStateSubscriber::UObjectStateSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/mujoco/object_states");
  CommonSubscriberParameters.MessageType = TEXT("mujoco_msgs/ObjectState");
}

void UObjectStateSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogObjectStateSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }
  if (UObjectController *ObjectController = GetRoboManager()->GetObjectController())
  {
    Subscriber = MakeShareable<FObjectStateSubscriberCallback>(
        new FObjectStateSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, ObjectController));
  }
}

FObjectStateSubscriberCallback::FObjectStateSubscriberCallback(
    FString InTopic, FString InType, UObjectController *InObjectController) : FROSBridgeSubscriber(InTopic, InType)
{
  ObjectController = InObjectController;
}

TSharedPtr<FROSBridgeMsg> FObjectStateSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<mujoco_msgs::ObjectState> ObjectState =
      MakeShareable<mujoco_msgs::ObjectState>(new mujoco_msgs::ObjectState());

  ObjectState->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(ObjectState);
}

void FObjectStateSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (ObjectController != nullptr)
  {
    TSharedPtr<mujoco_msgs::ObjectState> ObjectState = StaticCastSharedPtr<mujoco_msgs::ObjectState>(Msg);

    if (AActor *Object = ObjectController->GetObjectInMujoco(ObjectState->GetName()))
    {
      ObjectController->MoveObjectByMujoco(Object, *ObjectState);
    }
  }
}