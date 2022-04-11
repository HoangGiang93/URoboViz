#include "ROS/Publisher/TFPublisher.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "tf2_msgs/TFMessage.h"

DEFINE_LOG_CATEGORY_STATIC(LogTFPublisher, Log, All)

UTFPublisher::UTFPublisher()
{
  CommonPublisherParameters.Topic = TEXT("/tf");
  CommonPublisherParameters.MessageType = TEXT("tf2_msgs/TFMessage");
}

void UTFPublisher::Publish()
{
  TSharedPtr<tf2_msgs::TFMessage> TfMessage =
      MakeShareable(new tf2_msgs::TFMessage());

  geometry_msgs::Transform ObjectTransfMsg;
  geometry_msgs::TransformStamped ObjectFrame;

  UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AStaticMeshActor::StaticClass(), TEXT("tf"), Objects);
  for (AActor *const Object : Objects)
  {
    ObjectFrame.SetHeader(std_msgs::Header(Seq, FROSTime(), FrameId));
    ObjectFrame.SetChildFrameId(Object->GetName());

    geometry_msgs::Transform TransfMsgTemp(
        geometry_msgs::Vector3(FConversions::UToROS(Object->GetActorLocation())),
        geometry_msgs::Quaternion(FConversions::UToROS(Object->GetActorQuat())));
    ObjectTransfMsg = TransfMsgTemp;
    ObjectFrame.SetTransform(ObjectTransfMsg);
    TfMessage->AddTransform(ObjectFrame);
  }

  Handler->PublishMsg(CommonPublisherParameters.Topic, TfMessage);
  Handler->Process();

  Seq++;
}
