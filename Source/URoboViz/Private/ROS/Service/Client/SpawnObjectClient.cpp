// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Service/Client/SpawnObjectClient.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TriggerBase.h"
#include "ObjectController.h"
#include "mujoco_srvs/SpawnObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpawnObjectClient, Log, All)

TSet<AActor *> ObjectsToSpawnInMujoco;

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

    FTimerHandle UnusedHandle;
    FTimerDelegate TimerCallback;
    TimerCallback.BindLambda([TriggerBase]
                             {
                               TSet<AActor *> OverlappingActors;
                               TriggerBase->GetOverlappingActors(OverlappingActors);
                               UE_LOG(LogSpawnObjectClient, Log, TEXT("Try to spawn %d objects"), OverlappingActors.Num())
                               for (AActor *const OverlappingActor : OverlappingActors)
                               {
                                 if (AStaticMeshActor *StaticMeshActor = Cast<AStaticMeshActor>(OverlappingActor))
                                 {
                                   ObjectsToSpawnInMujoco.Add(StaticMeshActor);
                                 }
                               } });

    GetWorld()->GetTimerManager().SetTimer(UnusedHandle, TimerCallback, 1.f, false);
  }
}

void USpawnObjectClient::CreateServiceClient()
{
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogSpawnObjectClient, Warning, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }

  ServiceClient = MakeShareable<FSpawnObjectClientCallback>(new FSpawnObjectClientCallback(CommonServiceClientParameters.ServiceName, CommonServiceClientParameters.ServiceType, GetRoboManager()->GetObjectController()));
  Request = MakeShareable(new mujoco_srvs::SpawnObject::Request());
  Response = MakeShareable(new mujoco_srvs::SpawnObject::Response());
}

void USpawnObjectClient::Tick()
{
  if (ObjectsToSpawnInMujoco.Num() == 0 || GetRoboManager() == nullptr || GetRoboManager()->GetObjectController() == nullptr)
  {
    return;
  }

  static TMap<FString, mujoco_msgs::ObjectInfo::EType> TypeMap =
      {
          {TEXT("Cube"), mujoco_msgs::ObjectInfo::CUBE},
          {TEXT("Cylinder"), mujoco_msgs::ObjectInfo::CYLINDER},
          {TEXT("Sphere"), mujoco_msgs::ObjectInfo::SPHERE}};

  TArray<mujoco_msgs::ObjectStatus> ObjectStatusArray;
  int32 i = 0;
  TSet<AActor *> Objects;
  for (AActor *const Object : ObjectsToSpawnInMujoco)
  {
    if (i++ > SpawnObjectCountPerCycle)
    {
      break;
    }

    if (AStaticMeshActor *StaticMeshObject = Cast<AStaticMeshActor>(Object))
    {
      if (StaticMeshObject->GetStaticMeshComponent() != nullptr &&
          GetRoboManager()->GetObjectController()->GetObjectInMujoco(Object->GetName()) == nullptr)
      {
        mujoco_msgs::ObjectStatus ObjectStatus;
        mujoco_msgs::ObjectInfo ObjectInfo;
        geometry_msgs::Pose Pose;
        geometry_msgs::Twist Velocity;

        ObjectInfo.SetName(StaticMeshObject->GetName());
        if (bSpawnBoundingBox)
        {
          ObjectInfo.SetType(mujoco_msgs::ObjectInfo::CUBE);
        }
        else
        {
          ObjectInfo.SetType(mujoco_msgs::ObjectInfo::MESH);
          for (const TPair<FString, mujoco_msgs::ObjectInfo::EType> &Type : TypeMap)
          {
            if (StaticMeshObject->GetStaticMeshComponent()->GetStaticMesh()->GetName().Contains(Type.Key))
            {
              ObjectInfo.SetType(Type.Value);
              break;
            }
          }
        }

        switch (StaticMeshObject->GetStaticMeshComponent()->Mobility)
        {
        case EComponentMobility::Static:
          ObjectInfo.SetMovable(false);
          break;

        case EComponentMobility::Stationary:
          ObjectInfo.SetMovable(false);
          break;

        case EComponentMobility::Movable:
          ObjectInfo.SetMovable(true);
          break;

        default:
          break;
        }
        if (bSpawnBoundingBox)
        {
          const FVector ObjectLocation = StaticMeshObject->GetActorLocation();
          const FRotator ObjectRotator = StaticMeshObject->GetActorRotation();
          StaticMeshObject->SetActorLocationAndRotation(FVector(1000.f, 0.f, 0.f), FRotator()); // Move the object to somewhere that doesn't collide to anything
          ObjectInfo.SetSize(FConversions::CmToM(StaticMeshObject->GetComponentsBoundingBox().GetSize() / 2));
          StaticMeshObject->SetActorLocationAndRotation(ObjectLocation, ObjectRotator);
        }
        else
        {
          ObjectInfo.SetSize(StaticMeshObject->GetActorScale3D() / 2);
        }

        FLinearColor Color(1.f, 1.f, 1.f, 1.f);
        if (StaticMeshObject->GetStaticMeshComponent()->GetMaterial(0) != nullptr)
        {
          StaticMeshObject->GetStaticMeshComponent()->GetMaterial(0)->GetVectorParameterValue(TEXT("BaseColor"), Color);
        }
        ObjectInfo.SetColor(std_msgs::ColorRGBA(Color.R, Color.G, Color.B, Color.A));
        geometry_msgs::Inertia Inertial;
        Inertial.SetM(StaticMeshObject->GetStaticMeshComponent()->GetMass());
        Inertial.SetCom(FConversions::CmToM(StaticMeshObject->GetStaticMeshComponent()->GetCenterOfMass() - StaticMeshObject->GetActorLocation()));
        Inertial.SetIxx(StaticMeshObject->GetStaticMeshComponent()->GetInertiaTensor().X / 10000);
        Inertial.SetIyy(StaticMeshObject->GetStaticMeshComponent()->GetInertiaTensor().Y / 10000);
        Inertial.SetIzz(StaticMeshObject->GetStaticMeshComponent()->GetInertiaTensor().Z / 10000);
        Inertial.SetIxy(0.0);
        Inertial.SetIxz(0.0);
        Inertial.SetIyz(0.0);
        ObjectInfo.SetInertial(Inertial);
        FString Mesh = StaticMeshObject->GetStaticMeshComponent()->GetStaticMesh()->GetName();
        Mesh.RemoveFromStart(TEXT("SM_"));
        ObjectInfo.SetMesh(Mesh);
        ObjectStatus.SetInfo(ObjectInfo);

        Pose.SetPosition(FConversions::UToROS(StaticMeshObject->GetActorLocation()));
        Pose.SetOrientation(FConversions::UToROS(StaticMeshObject->GetActorRotation().Quaternion()));
        ObjectStatus.SetPose(Pose);

        Velocity.SetLinear(FConversions::UToROS(StaticMeshObject->GetStaticMeshComponent()->GetPhysicsLinearVelocity()));
        Velocity.SetAngular(StaticMeshObject->GetActorRotation().UnrotateVector(StaticMeshObject->GetStaticMeshComponent()->GetPhysicsAngularVelocityInRadians()) * FVector(-1, 1, -1));
        ObjectStatus.SetVelocity(Velocity);

        ObjectStatusArray.Add(ObjectStatus);

        Objects.Add(StaticMeshObject);
      }
    }
  }

  StaticCastSharedPtr<mujoco_srvs::SpawnObject::Request>(Request)->SetObjects(ObjectStatusArray);

  Super::CallService();

  GetRoboManager()->GetObjectController()->AddObjectsInMujoco(Objects);

  for (AActor *const Object : Objects)
  {
    ObjectsToSpawnInMujoco.Remove(Object);
  }
}

void USpawnObjectClient::OnBeginOverlap(AActor *OverlappedActor, AActor *OtherActor)
{
  if (AStaticMeshActor *const Object = Cast<AStaticMeshActor>(OtherActor))
  {
    ObjectsToSpawnInMujoco.Add(Object);
  }
}

FSpawnObjectClientCallback::FSpawnObjectClientCallback(const FString &InServiceName, const FString &InServiceType, UObjectController *InObjectController) : FROSBridgeSrvClient(InServiceName, InServiceType)
{
  ObjectController = InObjectController;
}

void FSpawnObjectClientCallback::Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse)
{
  if (ObjectController == nullptr)
  {
    return;
  }

  for (const FString &ObjectName : StaticCastSharedPtr<mujoco_srvs::SpawnObject::Response>(InResponse)->GetNames())
  {
    if (AStaticMeshActor *StaticMeshObject = Cast<AStaticMeshActor>(ObjectController->GetObjectInMujoco(ObjectName)))
    {
      if (StaticMeshObject->GetStaticMeshComponent()->Mobility == EComponentMobility::Movable)
      {
        StaticMeshObject->GetStaticMeshComponent()->SetSimulatePhysics(false);
      }
    }
  }
}