// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Service/Client/SpawnObjectClient.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TriggerBase.h"
#include "ObjectController.h"
#include "mujoco_srvs/SpawnObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpawnObjectClient, Log, All)

USpawnObjectClient::USpawnObjectClient()
{
  CommonServiceClientParameters.ServiceName = TEXT("/mujoco/spawn_objects");
  CommonServiceClientParameters.ServiceType = TEXT("mujoco_srvs/SpawnObject");
}

void USpawnObjectClient::Init()
{
  Super::Init();
  for (ATriggerBase *TriggerBase : TriggerBases)
  {
    if (TriggerBase == nullptr)
    {
      continue;
    }
    TriggerBase->OnActorBeginOverlap.AddDynamic(this, &USpawnObjectClient::OnBeginOverlap);
  }
}

void USpawnObjectClient::CreateServiceClient()
{
  ServiceClient = MakeShareable<FSpawnObjectClientCallback>(new FSpawnObjectClientCallback(CommonServiceClientParameters.ServiceName, CommonServiceClientParameters.ServiceType));
  Request = MakeShareable(new mujoco_srvs::SpawnObject::Request());
  Response = MakeShareable(new mujoco_srvs::SpawnObject::Response());
}

void USpawnObjectClient::CallService(const TSet<AStaticMeshActor *> &Objects)
{
  static TMap<FString, mujoco_msgs::ObjectInfo::EType> TypeMap =
      {
          {TEXT("Cube"), mujoco_msgs::ObjectInfo::CUBE},
          {TEXT("Cylinder"), mujoco_msgs::ObjectInfo::CYLINDER},
          {TEXT("Sphere"), mujoco_msgs::ObjectInfo::SPHERE}};

  TArray<mujoco_msgs::ObjectStatus> ObjectStatusArray;
  ObjectStatusArray.Reserve(Objects.Num());
  std_msgs::Header Header(Seq++, FROSTime(), FrameId);

  bool bShouldCallService = false;
  for (AStaticMeshActor *const Object : Objects)
  {
    if (Object != nullptr &&
        Object->GetStaticMeshComponent() != nullptr &&
        GetRoboManager()->GetObjectController() != nullptr &&
        GetRoboManager()->GetObjectController()->GetObjectInMujoco(Object->GetName()) == nullptr)
    {
      bShouldCallService = true;

      mujoco_msgs::ObjectStatus ObjectStatus;
      mujoco_msgs::ObjectInfo ObjectInfo;
      geometry_msgs::Pose Pose;
      geometry_msgs::Twist Velocity;

      ObjectStatus.SetHeader(Header);

      ObjectInfo.SetName(Object->GetName());
      ObjectInfo.SetType(mujoco_msgs::ObjectInfo::MESH);
      for (const TPair<FString, mujoco_msgs::ObjectInfo::EType> &Type : TypeMap)
      {
        if (Object->GetName().Contains(Type.Key))
        {
          ObjectInfo.SetType(Type.Value);
          break;
        }
      }
      ObjectInfo.SetSize(Object->GetActorScale3D() / 2);
      FLinearColor Color(1.f, 1.f, 1.f, 1.f);
      if (Object->GetStaticMeshComponent() != nullptr && Object->GetStaticMeshComponent()->GetMaterial(0))
      {
        Object->GetStaticMeshComponent()->GetMaterial(0)->GetVectorParameterValue(TEXT("BaseColor"), Color);
      }
      ObjectInfo.SetColor(std_msgs::ColorRGBA(Color.R, Color.G, Color.B, 1-Color.A));
      geometry_msgs::Inertia Inertial;
      Inertial.SetM(Object->GetStaticMeshComponent()->GetMass());
      Inertial.SetCom(FConversions::CmToM(Object->GetStaticMeshComponent()->GetCenterOfMass() - Object->GetActorLocation()));
      Inertial.SetIxx(Object->GetStaticMeshComponent()->GetInertiaTensor().X / 10000);
      Inertial.SetIyy(Object->GetStaticMeshComponent()->GetInertiaTensor().Y / 10000);
      Inertial.SetIzz(Object->GetStaticMeshComponent()->GetInertiaTensor().Z / 10000);
      Inertial.SetIxy(0.0);
      Inertial.SetIxz(0.0);
      Inertial.SetIyz(0.0);
      ObjectInfo.SetInertial(Inertial);
      FString Mesh = Object->GetStaticMeshComponent()->GetStaticMesh()->GetName();
      Mesh.RemoveFromStart(TEXT("SM_"));
      Mesh.Append(TEXT(".xml"));
      ObjectInfo.SetMesh(Mesh);
      ObjectStatus.SetInfo(ObjectInfo);

      Pose.SetPosition(FConversions::UToROS(Object->GetActorLocation()));
      Pose.SetOrientation(FConversions::UToROS(Object->GetActorRotation().Quaternion()));
      ObjectStatus.SetPose(Pose);

      Velocity.SetLinear(FConversions::UToROS(Object->GetStaticMeshComponent()->GetPhysicsLinearVelocity()));
      Velocity.SetAngular(Object->GetStaticMeshComponent()->GetPhysicsAngularVelocityInRadians() * FVector(-1, 1, -1));
      ObjectStatus.SetVelocity(Velocity);

      GetRoboManager()->GetObjectController()->AddObjectInMujoco(Object);

      ObjectStatusArray.Add(ObjectStatus);
    }
  }

  StaticCastSharedPtr<mujoco_srvs::SpawnObject::Request>(Request)->SetObjects(ObjectStatusArray);

  if (bShouldCallService)
  {
    Super::CallService();
  }
}

void USpawnObjectClient::OnBeginOverlap(AActor *OverlappedActor, AActor *OtherActor)
{
  if (AStaticMeshActor *const Object = Cast<AStaticMeshActor>(OtherActor))
  {
    CallService({Object});
  }
}

FSpawnObjectClientCallback::FSpawnObjectClientCallback(const FString &InServiceName, const FString &InServiceType) : FROSBridgeSrvClient(InServiceName, InServiceType)
{
}

void FSpawnObjectClientCallback::Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse)
{
}