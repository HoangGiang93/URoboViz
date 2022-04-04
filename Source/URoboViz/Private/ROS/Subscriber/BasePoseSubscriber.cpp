#include "ROS/Subscriber/BasePoseSubscriber.h"
#include "Controllers/BaseController.h"
#include "sensor_msgs/JointState.h"
#include "tf2_msgs/TFMessage.h"

DEFINE_LOG_CATEGORY_STATIC(LogBasePoseSubscriber, Log, All)

UBasePoseSubscriber::UBasePoseSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/tf");
  CommonSubscriberParameters.MessageType = TEXT("tf2_msgs/TFMessage");
}

void UBasePoseSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    return;
  }

  if (UBaseController *BaseController = Cast<UBaseController>(GetRoboManager()->GetController(BaseControllerName)))
  {
    Subscriber = MakeShareable<FBasePoseSubscriberCallback>(
        new FBasePoseSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, BaseController, BaseName));
  }
  else
  {
    UE_LOG(LogBasePoseSubscriber, Warning, TEXT("%s of %s not found"), *BaseControllerName, *GetName())
  }
}

FBasePoseSubscriberCallback::FBasePoseSubscriberCallback(
    FString InTopic, FString InType, UBaseController *InBaseController, FString InBaseName) : FROSBridgeSubscriber(InTopic, InType)
{
  BaseController = InBaseController;
  BaseName = InBaseName;
}

TSharedPtr<FROSBridgeMsg> FBasePoseSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<tf2_msgs::TFMessage> TF =
      MakeShareable<tf2_msgs::TFMessage>(new tf2_msgs::TFMessage());

  TF->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(TF);
}

void FBasePoseSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if (BaseController)
  {
    TSharedPtr<tf2_msgs::TFMessage> TF = StaticCastSharedPtr<tf2_msgs::TFMessage>(Msg);
    for (const geometry_msgs::TransformStamped &Transform : TF->GetTransforms())
    {
      if (Transform.GetChildFrameId().Compare(BaseName) == 0)
      {
        FVector BaseLocation = Transform.GetTransform().GetTranslation().GetVector();
        FQuat BaseRotation = Transform.GetTransform().GetRotation().GetQuat();
        BaseController->SetDesiredBasePoseFromROS(BaseLocation, BaseRotation);
        break;
      }
    }
  }
}