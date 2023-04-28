// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Publisher/ObjectMarkerArrayPublisher.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "ObjectController.h"
#include "RoboManager.h"
#include "visualization_msgs/MarkerArray.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectMarkerArrayPublisher, Log, All)

UObjectMarkerArrayPublisher::UObjectMarkerArrayPublisher()
{
  CommonPublisherParameters.Topic = TEXT("/mujoco/visualization_marker_array");
  CommonPublisherParameters.MessageType = TEXT("visualization_msgs/MarkerArray");
}

void UObjectMarkerArrayPublisher::Publish()
{
  if (GetRoboManager() == nullptr || GetRoboManager()->GetObjectController() == nullptr)
  {
    UE_LOG(LogObjectMarkerArrayPublisher, Error, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  if (GetRoboManager()->GetObjectController()->GetObjectsInUnreal().Num() == 0)
  {
    return;
  }

  TSharedPtr<visualization_msgs::MarkerArray> MarkerArrayMessage =
      MakeShareable(new visualization_msgs::MarkerArray());

  MarkerArrayMessage->GetMarkers().Empty();
  for (AActor *const Object : GetRoboManager()->GetObjectController()->GetObjectsInUnreal())
  {
    visualization_msgs::Marker Marker;

    Marker.SetActionTye(visualization_msgs::Marker::EAction::MODIFY);

    Marker.SetHeader(std_msgs::Header(Seq, FROSTime(), FrameId));

    if (Object->ActorHasTag(TEXT("Cube")))
    {
      Marker.SetMarkeType(visualization_msgs::Marker::EType::CUBE);
      Marker.bMeshUseEmbeddedMaterials = false;
    }
    else if (Object->ActorHasTag(TEXT("Sphere")))
    {
      Marker.SetMarkeType(visualization_msgs::Marker::EType::SPHERE);
      Marker.bMeshUseEmbeddedMaterials = false;
    }
    else if (Object->ActorHasTag(TEXT("Cylinder")))
    {
      Marker.SetMarkeType(visualization_msgs::Marker::EType::CYLINDER);
      Marker.bMeshUseEmbeddedMaterials = false;
    }
    else
    {
      Marker.SetMarkeType(visualization_msgs::Marker::EType::MESH_RESOURCE);

      FString ObjectName = Object->GetName();
      ObjectName.RemoveFromEnd(TEXT(".xml"));
      
      for (FString MeshResourceDir : MeshResourceDirs)
      {
        MeshResourceDir += ObjectName + "/dae/" + ObjectName + ".dae";
        Marker.SetMeshResource(MeshResourceDir);
        Marker.bMeshUseEmbeddedMaterials = true;
        break;
        // if (FPaths::FileExists(MeshResourceDir))
        // {
        //   Marker.SetMeshResource(MeshResourceDir);
        //   break;
        // }
        // UE_LOG(LogObjectMarkerArrayPublisher, Warning, TEXT("%s not found"), *MeshResourceDir)
      }
    }
    
    Marker.SetScale(geometry_msgs::Vector3(Object->GetActorScale3D()));

    Marker.SetNamespace(Object->GetName());

    Marker.SetId(0);

    FLinearColor Color(1.f, 1.f, 1.f, 1.f);
    if (UMaterialInterface *Material = Cast<AStaticMeshActor>(Object)->GetStaticMeshComponent()->GetMaterial(0))
    {
      Material->GetVectorParameterValue(TEXT("BaseColor"), Color);
    }
    Marker.SetColor(std_msgs::ColorRGBA(Color));

    Marker.SetPose(geometry_msgs::Pose(geometry_msgs::Point(FConversions::UToROS(Object->GetActorLocation())), geometry_msgs::Quaternion(FConversions::UToROS(Object->GetActorQuat()))));

    MarkerArrayMessage->AddMarker(Marker);
  }

  Handler->PublishMsg(CommonPublisherParameters.Topic, MarkerArrayMessage);

  Seq++;
}
