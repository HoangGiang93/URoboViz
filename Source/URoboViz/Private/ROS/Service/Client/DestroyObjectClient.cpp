// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Service/Client/DestroyObjectClient.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TriggerBase.h"
#include "ObjectController.h"
#include "mujoco_srvs/DestroyObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogDestroyObjectClient, Log, All)

TSet<AActor *> ObjectsToDestroyInMujoco;

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
  if (ObjectsToDestroyInMujoco.Num() == 0 || GetRoboManager() == nullptr || GetRoboManager()->GetObjectController() == nullptr)
  {
    return;
  }

  TArray<FString> ObjectNames;
  ObjectNames.Reserve(ObjectsToDestroyInMujoco.Num());
  for (AActor *const Object : ObjectsToDestroyInMujoco)
  {
    if (Object != nullptr &&
        GetRoboManager()->GetObjectController()->GetObjectInMujoco(Object->GetName()) != nullptr)
    {
      ObjectNames.Add(Object->GetName());
    }
  }

  StaticCastSharedPtr<mujoco_srvs::DestroyObject::Request>(Request)->SetNames(ObjectNames);

  Super::CallService();

  GetRoboManager()->GetObjectController()->RemoveObjectInMujoco(ObjectsToDestroyInMujoco);
  ObjectsToDestroyInMujoco.Empty();
}

void UDestroyObjectClient::OnEndOverlap(AActor *OverlappedActor, AActor *OtherActor)
{
  if (AStaticMeshActor *Object = Cast<AStaticMeshActor>(OtherActor))
  {
    TSet<AActor *> OverlappingActors;
    Object->GetOverlappingActors(OverlappingActors);
    if (!OverlappingActors.Array().ContainsByPredicate([&](const AActor *const Actor)
                                                       { return Cast<ATriggerBase>(Actor) != nullptr; }) ||
        OverlappingActors.Num() == 0)
    {
      if (Object->GetStaticMeshComponent()->Mobility == EComponentMobility::Movable)
      {
        Object->GetStaticMeshComponent()->SetSimulatePhysics(true);
      }
      ObjectsToDestroyInMujoco.Add(Object);
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
    if (AStaticMeshActor *Object = Cast<AStaticMeshActor>(ObjectController->GetObjectInMujoco(ObjectState.GetName())))
    {
      ObjectController->MoveObjectByMujoco(Object, ObjectState);
    }
  }
}