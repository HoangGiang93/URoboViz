// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/ObjectStateArraySubscriber.h"
#include "Engine/StaticMeshActor.h"
#include "ObjectController.h"
#include "mujoco_msgs/ObjectStateArray.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectStateArraySubscriber, Log, All)

UObjectStateArraySubscriber::UObjectStateArraySubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/mujoco/object_states");
  CommonSubscriberParameters.MessageType = TEXT("mujoco_msgs/ObjectStateArray");
}

void UObjectStateArraySubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogObjectStateArraySubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }
  if (UObjectController *ObjectController = GetRoboManager()->GetObjectController())
  {
    Subscriber = MakeShareable<FObjectStateArraySubscriberCallback>(
        new FObjectStateArraySubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, ObjectController));
  }
}

FObjectStateArraySubscriberCallback::FObjectStateArraySubscriberCallback(
    FString InTopic, FString InType, UObjectController *InObjectController) : FROSBridgeSubscriber(InTopic, InType)
{
  ObjectController = InObjectController;
}

TSharedPtr<FROSBridgeMsg> FObjectStateArraySubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<mujoco_msgs::ObjectStateArray> ObjectStateArray =
      MakeShareable<mujoco_msgs::ObjectStateArray>(new mujoco_msgs::ObjectStateArray());

  ObjectStateArray->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(ObjectStateArray);
}

void FObjectStateArraySubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (ObjectController != nullptr)
  {
    for (const mujoco_msgs::ObjectState &ObjectState : StaticCastSharedPtr<mujoco_msgs::ObjectStateArray>(Msg)->GetObjectStates())
    {
      if (AActor *Object = ObjectController->GetObjectInMujoco(ObjectState.GetName()))
      {
        ObjectController->MoveObjectByMujoco(Object, ObjectState);
      }
    }
  }
}