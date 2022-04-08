// Fill out your copyright notice in the Description page of Project Settings.

#include "ROS/Service/Client/SpawnObjectClient.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/BoxComponent.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "ObjectController.h"
#include "mujoco_srvs/SpawnObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpawnObjectClient, Log, All)

USpawnObjectClient::USpawnObjectClient()
{
  CommonServiceClientParameters.ServiceName = TEXT("/spawn_objects");
  CommonServiceClientParameters.ServiceType = TEXT("mujoco_msgs/SpawnObject");
}

void USpawnObjectClient::Init()
{
  Super::Init();
  for (ASkeletalMeshActor *Robot : Robots)
  {
    UBoxComponent *TriggerVolume = NewObject<UBoxComponent>(Robot, FName(Robot->GetName() + TEXT("_TriggerVolume")));
    TriggerVolume->SetBoxExtent(TriggerVolumeSize);
    TriggerVolume->SetGenerateOverlapEvents(true);
    TriggerVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    TriggerVolume->AttachToComponent(Robot->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    TriggerVolume->AddLocalOffset(FVector(0.f, 0.f, TriggerVolumeSize.Z));
    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &USpawnObjectClient::OnBeginOverlap);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &USpawnObjectClient::OnEndOverlap);
    TriggerVolume->RegisterComponent();
  }
}

void USpawnObjectClient::CreateServiceClient()
{
  ServiceClient = MakeShareable<FSpawnObjectClientCallback>(new FSpawnObjectClientCallback(CommonServiceClientParameters.ServiceName, CommonServiceClientParameters.ServiceType));
  Request = MakeShareable(new mujoco_srvs::SpawnObject::Request());
  Response = MakeShareable(new mujoco_srvs::SpawnObject::Response());
}

void USpawnObjectClient::CallService(const TArray<AStaticMeshActor *> &Objects)
{
  static TMap<FString, mujoco_msgs::ModelState::EType> TypeMap =
      {
          {TEXT("Cube"), mujoco_msgs::ModelState::EType::CUBE},
          {TEXT("Cylinder"), mujoco_msgs::ModelState::EType::CYLINDER},
          {TEXT("Sphere"), mujoco_msgs::ModelState::EType::SPHERE},
          {TEXT(""), mujoco_msgs::ModelState::EType::MESH}};

  int32 ObjectNum = Objects.Num();
  TArray<mujoco_msgs::ModelState> ModelStates;
  ModelStates.Init(mujoco_msgs::ModelState(), ObjectNum);
  std_msgs::Header Header(Seq++, FROSTime(), FrameId);
  for (int32 i = 0; i < ObjectNum; i++)
  {
    if (Objects[i] != nullptr && Objects[i]->GetStaticMeshComponent() != nullptr)
    {
      ModelStates[i].SetHeader(Header);
      ModelStates[i].SetName(Objects[i]->GetName());
      geometry_msgs::Pose Pose;
      Pose.SetPosition(FConversions::UToROS(Objects[i]->GetActorLocation()));
      Pose.SetOrientation(FConversions::UToROS(Objects[i]->GetActorRotation().Quaternion()));
      ModelStates[i].SetPose(Pose);
      geometry_msgs::Twist Velocity;
      Velocity.SetLinear(FConversions::UToROS(Objects[i]->GetStaticMeshComponent()->GetPhysicsLinearVelocity()));
      Velocity.SetAngular(Objects[i]->GetStaticMeshComponent()->GetPhysicsAngularVelocityInRadians());
      ModelStates[i].SetVelocity(Velocity);

      for (const TPair<FString, mujoco_msgs::ModelState::EType> &Type : TypeMap)
      {
        if (Objects[i]->GetName().Contains(Type.Key))
        {
          ModelStates[i].SetType(Type.Value);
          break;
        }
      }

      ModelStates[i].SetScale(Objects[i]->GetActorScale3D()/2);
      FLinearColor Color(1.f, 1.f, 1.f, 1.f);
      if (Objects[i]->GetStaticMeshComponent()->GetStaticMesh() != nullptr && Objects[i]->GetStaticMeshComponent()->GetStaticMesh()->GetMaterial(0))
      {
        Objects[i]->GetStaticMeshComponent()->GetStaticMesh()->GetMaterial(0)->GetVectorParameterValue(TEXT("BaseColor"), Color);
      }
      ModelStates[i].SetColor(std_msgs::ColorRGBA(Color.R, Color.G, Color.B, Color.A));

      GetRoboManager()->GetObjectController()->AddObject(Objects[i]);
    }
  }

  StaticCastSharedPtr<mujoco_srvs::SpawnObject::Request>(Request)->SetModelStates(ModelStates);

  Super::CallService();
}

void USpawnObjectClient::Tick()
{
  Super::Tick();
}

void USpawnObjectClient::OnBeginOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
  if (AStaticMeshActor *Object = Cast<AStaticMeshActor>(OtherActor))
  {
    UE_LOG(LogSpawnObjectClient, Log, TEXT("%s entered"), *OtherActor->GetName());
    CallService({Object});
  }
}

void USpawnObjectClient::OnEndOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
  UE_LOG(LogSpawnObjectClient, Log, TEXT("%s left"), *OtherActor->GetName());
}

FSpawnObjectClientCallback::FSpawnObjectClientCallback(const FString &InServiceName, const FString &InServiceType) : FROSBridgeSrvClient(InServiceName, InServiceType)
{
}

void FSpawnObjectClientCallback::Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse)
{
  UE_LOG(LogSpawnObjectClient, Log, TEXT("%s"), *InResponse->ToString())
}