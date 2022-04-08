#include "ROS/Subscriber/JointStateSubscriber.h"
#include "Controllers/JointController.h"
#include "sensor_msgs/JointState.h"

DEFINE_LOG_CATEGORY_STATIC(LogJointStateSubscriber, Log, All)

UJointStateSubscriber::UJointStateSubscriber()
{
  CommonSubscriberParameters.Topic = TEXT("/joint_states");
  CommonSubscriberParameters.MessageType = TEXT("sensor_msgs/JointState");
}

void UJointStateSubscriber::CreateSubscriber()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogJointStateSubscriber, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  if (UJointController *JointController = Cast<UJointController>(GetRoboManager()->GetController(JointControllerName)))
  {
    Subscriber = MakeShareable<FJointStateSubscriberCallback>(
        new FJointStateSubscriberCallback(CommonSubscriberParameters.Topic, CommonSubscriberParameters.MessageType, JointController));
  }
  else
  {
    UE_LOG(LogJointStateSubscriber, Warning, TEXT("%s of %s not found"), *JointControllerName, *GetName())
  }
}

FJointStateSubscriberCallback::FJointStateSubscriberCallback(
    FString InTopic, FString InType, UJointController *InJointController) : FROSBridgeSubscriber(InTopic, InType)
{
  JointController = InJointController;
}

TSharedPtr<FROSBridgeMsg> FJointStateSubscriberCallback::ParseMessage(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<sensor_msgs::JointState> JointState =
      MakeShareable<sensor_msgs::JointState>(new sensor_msgs::JointState());

  JointState->FromJson(JsonObject);

  return StaticCastSharedPtr<FROSBridgeMsg>(JointState);
}

void FJointStateSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
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