// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Service/Client/DestroyObjectClient.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TriggerBase.h"
#include "ObjectController.h"
#include "mujoco_srvs/DestroyObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogDestroyObjectClient, Log, All)

UDestroyObjectClient::UDestroyObjectClient()
{
  CommonServiceClientParameters.ServiceName = TEXT("/mujoco/destroy_objects");
  CommonServiceClientParameters.ServiceType = TEXT("mujoco_srvs/DestroyObject");
}

void UDestroyObjectClient::Init()
{
  Super::Init();
  for (ATriggerBase *TriggerBase : TriggerBases)
  {
    if (TriggerBase == nullptr)
    {
      continue;
    }
    TriggerBase->OnActorEndOverlap.AddDynamic(this, &UDestroyObjectClient::OnEndOverlap);
  }
}

void UDestroyObjectClient::CreateServiceClient()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogDestroyObjectClient, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  ServiceClient = MakeShareable<FDestroyObjectClientCallback>(new FDestroyObjectClientCallback(CommonServiceClientParameters.ServiceName, CommonServiceClientParameters.ServiceType, GetRoboManager()->GetObjectController()));
  Request = MakeShareable(new mujoco_srvs::DestroyObject::Request());
  Response = MakeShareable(new mujoco_srvs::DestroyObject::Response());
}

void UDestroyObjectClient::Tick()
{
  if (GetRoboManager()->GetObjectController() == nullptr || GetRoboManager()->GetObjectController()->GetObjectsToRemoveInMujoco().Num() == 0)
  {
    return;
  }

  const TSet<AStaticMeshActor *> Objects = GetRoboManager()->GetObjectController()->GetObjectsToRemoveInMujoco();

  TArray<FString> ObjectNames;
  ObjectNames.Reserve(Objects.Num());
  for (AStaticMeshActor *const Object : Objects)
  {
    if (Object != nullptr &&
        GetRoboManager() != nullptr &&
        GetRoboManager()->GetObjectController() != nullptr &&
        GetRoboManager()->GetObjectController()->GetObjectInMujoco(Object->GetName()) != nullptr)
    {
      GetRoboManager()->GetObjectController()->RemoveObjectInMujoco(Object);
      ObjectNames.Add(Object->GetName());
    }
  }
  StaticCastSharedPtr<mujoco_srvs::DestroyObject::Request>(Request)->SetNames(ObjectNames);

  Super::CallService();
}

void UDestroyObjectClient::OnEndOverlap(AActor *OverlappedActor, AActor *OtherActor)
{
  if (GetRoboManager()->GetObjectController() == nullptr)
  {
    return;
  }

  if (AStaticMeshActor *Object = Cast<AStaticMeshActor>(OtherActor))
  {
    TSet<AActor *> OverlappingActors;
    Object->GetOverlappingActors(OverlappingActors);
    if (!OverlappingActors.Array().ContainsByPredicate([&](const AActor *const Actor){   
      return Cast<ATriggerBase>(Actor) != nullptr;
    }) || OverlappingActors.Num() == 0)
    {
      GetRoboManager()->GetObjectController()->RemoveObjectInMujoco(Object);
    }
  }
}

FDestroyObjectClientCallback::FDestroyObjectClientCallback(const FString &InServiceName, const FString &InServiceType, UObjectController *InObjectController) : FROSBridgeSrvClient(InServiceName, InServiceType)
{
  ObjectController = InObjectController;
}

void FDestroyObjectClientCallback::Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse)
{
  if (ObjectController == nullptr)
  {
    return;
  }

  for (const mujoco_msgs::ObjectState &ObjectState : StaticCastSharedPtr<mujoco_srvs::DestroyObject::Response>(InResponse)->GetObjectStates())
  {
    if (AStaticMeshActor *Object = ObjectController->GetObjectInMujoco(ObjectState.GetName()))
    {
      ObjectController->MoveObjectByMujoco(Object, ObjectState);
    }
  }
}