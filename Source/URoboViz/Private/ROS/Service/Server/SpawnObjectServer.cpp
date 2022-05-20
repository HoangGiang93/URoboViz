// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Service/Server/SpawnObjectServer.h"
#include "Async/Async.h"
#include "ObjectController.h"
#include "RoboManager.h"
#include "mujoco_srvs/SpawnObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpawnObjectServer, Log, All)

USpawnObjectServer::USpawnObjectServer()
{
  CommonServiceServerParameters.ServiceName = TEXT("/unreal/spawn_objects");
  CommonServiceServerParameters.ServiceType = TEXT("mujoco_msgs/SpawnObject");
}

void USpawnObjectServer::CreateServiceServer()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogSpawnObjectServer, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  ServiceServer =
      MakeShareable<FSpawnObjectServerCallback>(new FSpawnObjectServerCallback(CommonServiceServerParameters.ServiceName, CommonServiceServerParameters.ServiceType, GetRoboManager()->GetObjectController(), bAddObjectInMujoco));
}

FSpawnObjectServerCallback::FSpawnObjectServerCallback(const FString &InName, const FString &InType, UObjectController *InObjectController, const bool bInAddObjectInMujoco) : FROSBridgeSrvServer(InName, InType)
{
  ObjectController = InObjectController;
  bAddObjectInMujoco = bInAddObjectInMujoco;
}

TSharedPtr<FROSBridgeSrv::SrvRequest> FSpawnObjectServerCallback::FromJson(TSharedPtr<FJsonObject> JsonObject) const
{
  TSharedPtr<mujoco_srvs::SpawnObject::Request> Request =
      MakeShareable(new mujoco_srvs::SpawnObject::Request());
  Request->FromJson(JsonObject);
  return TSharedPtr<FROSBridgeSrv::SrvRequest>(Request);
}

TSharedPtr<FROSBridgeSrv::SrvResponse> FSpawnObjectServerCallback::Callback(TSharedPtr<FROSBridgeSrv::SrvRequest> InRequest)
{
  TSharedPtr<mujoco_srvs::SpawnObject::Request> Request =
      StaticCastSharedPtr<mujoco_srvs::SpawnObject::Request>(InRequest);
  if (ObjectController == nullptr)
  {
    return MakeShareable<FROSBridgeSrv::SrvResponse>(new mujoco_srvs::SpawnObject::Response());
  }

  bool bSuccess = true;
  TArray<mujoco_msgs::ObjectStatus> Objects = Request->GetObjects();
  TArray<FString> ObjectNames;
  ObjectNames.Reserve(Objects.Num());
  for (const mujoco_msgs::ObjectStatus &Object : Objects)
  {
    if (!ObjectController->SpawnOrMoveObjectByMujoco(Object, bAddObjectInMujoco))
    {
      bSuccess = false;
    }

    ObjectNames.Add(Object.GetInfo().GetName());
  }

  if (bSuccess)
  {
    return MakeShareable<FROSBridgeSrv::SrvResponse>(new mujoco_srvs::SpawnObject::Response(ObjectNames));
  }
  return MakeShareable<FROSBridgeSrv::SrvResponse>(new mujoco_srvs::SpawnObject::Response());
}