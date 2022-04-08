// Fill out your copyright notice in the Description page of Project Settings.

#include "ROS/Service/Server/SpawnObjectServer.h"
#include "Async/Async.h"
#include "ObjectController.h"
#include "RoboManager.h"
#include "mujoco_srvs/SpawnObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpawnObjectServer, Log, All)

USpawnObjectServer::USpawnObjectServer()
{
  CommonServiceServerParameters.ServiceName = TEXT("/spawn_objects");
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
      MakeShareable<FSpawnObjectServerCallback>(new FSpawnObjectServerCallback(CommonServiceServerParameters.ServiceName, CommonServiceServerParameters.ServiceType, GetRoboManager()->GetObjectController()));
}

FSpawnObjectServerCallback::FSpawnObjectServerCallback(const FString &InName, const FString &InType, UObjectController *InObjectController) : FROSBridgeSrvServer(InName, InType)
{
  ObjectController = InObjectController;
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
    return MakeShareable<FROSBridgeSrv::SrvResponse>(new mujoco_srvs::SpawnObject::Response(false));
  }

  bool bSuccess = true;
  TArray<mujoco_msgs::ModelState> ModelStates = Request->GetModelStates();
  for (const mujoco_msgs::ModelState &ModelState : ModelStates)
  {
    bSuccess = bSuccess && ObjectController->AddOrMoveObjectFromROS(ModelState);
  }

  return MakeShareable<FROSBridgeSrv::SrvResponse>(new mujoco_srvs::SpawnObject::Response(bSuccess));
}