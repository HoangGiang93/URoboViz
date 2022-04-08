#include "ROS/Subscriber/ModelMarkerSubscriber.h"
#include "ObjectController.h"
#include "visualization_msgs/Marker.h"

DEFINE_LOG_CATEGORY_STATIC(LogModelMarkerSubscriber, Log, All)

UModelMarkerSubscriber::UModelMarkerSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/visualization_marker");
  CommonSubscriberParameters.MessageType = TEXT("visualization_msgs/Marker");
}

void UModelMarkerSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogModelMarkerSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }
  if (UObjectController *ObjectController = GetRoboManager()->GetObjectController())
  {
    Subscriber = MakeShareable<FModelMarkerSubscriberCallback>(
        new FModelMarkerSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, ObjectController));
  }
}

FModelMarkerSubscriberCallback::FModelMarkerSubscriberCallback(
    FString InTopic, FString InType, UObjectController *InObjectController) : FROSBridgeSubscriber(InTopic, InType)
{
  ObjectController = InObjectController;
}

TSharedPtr<FROSBridgeMsg> FModelMarkerSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<visualization_msgs::Marker> ModelMarker =
      MakeShareable<visualization_msgs::Marker>(new visualization_msgs::Marker());

  ModelMarker->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(ModelMarker);
}

void FModelMarkerSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (ObjectController != nullptr)
  {
    TSharedPtr<visualization_msgs::Marker> ModelMarker = StaticCastSharedPtr<visualization_msgs::Marker>(Msg);

    const FString ObjectName = ModelMarker->GetNamespace();
    if (AStaticMeshActor *Object = ObjectController->GetObject(ObjectName))
    {
      ObjectController->MoveObjectFromROS(Object, *ModelMarker);
    }
  }
}