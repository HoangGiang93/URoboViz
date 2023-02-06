// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/ObjectMarkerSubscriber.h"
#include "Engine/StaticMeshActor.h"
#include "ObjectController.h"
#include "visualization_msgs/Marker.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectMarkerSubscriber, Log, All)

UObjectMarkerSubscriber::UObjectMarkerSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/mujoco/visualization_marker");
  CommonSubscriberParameters.MessageType = TEXT("visualization_msgs/Marker");
}

void UObjectMarkerSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogObjectMarkerSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }
  if (UObjectController *ObjectController = GetRoboManager()->GetObjectController())
  {
    Subscriber = MakeShareable<FObjectMarkerSubscriberCallback>(
        new FObjectMarkerSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, ObjectController));
  }
}

FObjectMarkerSubscriberCallback::FObjectMarkerSubscriberCallback(
    FString InTopic, FString InType, UObjectController *InObjectController) : FROSBridgeSubscriber(InTopic, InType)
{
  ObjectController = InObjectController;
}

TSharedPtr<FROSBridgeMsg> FObjectMarkerSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<visualization_msgs::Marker> ObjectMarker =
      MakeShareable<visualization_msgs::Marker>(new visualization_msgs::Marker());

  ObjectMarker->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(ObjectMarker);
}

void FObjectMarkerSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (ObjectController != nullptr)
  {
    TSharedPtr<visualization_msgs::Marker> ObjectMarker = StaticCastSharedPtr<visualization_msgs::Marker>(Msg);

    const FString ObjectName = ObjectMarker->GetNamespace();
    if (AActor *Object = ObjectController->GetObjectInMujoco(ObjectName))
    {
      ObjectController->MoveObjectByMarker(Object, *ObjectMarker);
    }
  }
}