// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectController.h"
#include "Async/Async.h"
#include "Async/AsyncWork.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "mujoco_msgs/ModelState.h"
#include "visualization_msgs/Marker.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectController, Log, All);

static FTransform GetTransform(const mujoco_msgs::ModelState &ModelState)
{
	const FVector Location = ModelState.GetPose().GetPosition().GetVector();
	const FRotator Rotation = ModelState.GetPose().GetOrientation().GetQuat().Rotator();
	const FVector Scale = ModelState.GetScale();
	return FTransform(Rotation, Location, Scale);
}

static FTransform GetTransform(const visualization_msgs::Marker &ModelMarker)
{
	const FVector Location = ModelMarker.GetPose().GetPosition().GetVector();
	const FRotator Rotation = ModelMarker.GetPose().GetOrientation().GetQuat().Rotator();
	const FVector Scale = ModelMarker.GetScale().GetVector();
	return FTransform(Rotation, Location, Scale);
}

UObjectController::UObjectController()
{
	ColorMap = {
			{FVector4(0, 0, 1, 1), TEXT("Blue")},
			{FVector4(0, 1, 1, 1), TEXT("Cyan")},
			{FVector4(0, 1, 0, 1), TEXT("Green")},
			{FVector4(1, 0, 0.5, 1), TEXT("Pink")},
			{FVector4(0.5, 0, 1, 1), TEXT("Purple")},
			{FVector4(1, 0, 0, 1), TEXT("Red")},
			{FVector4(1, 1, 0, 1), TEXT("Yellow")},
			{FVector4(0.8, 0.1, 0, 1), TEXT("Orange")}};
}

AStaticMeshActor *UObjectController::GetObject(const FString &ObjectName) const
{
	AStaticMeshActor *const *ObjectPtr = Objects.FindByPredicate([&](AStaticMeshActor *Object)
																															 { return Object->GetName().Compare(ObjectName) == 0; });
	if (ObjectPtr != nullptr)
	{
		return *ObjectPtr;
	}
	else
	{
		return nullptr;
	}
}

void UObjectController::AddObject(AStaticMeshActor *const Object)
{ 
	if (Object != nullptr && Object->GetStaticMeshComponent() != nullptr)
	{
		Object->GetStaticMeshComponent()->SetSimulatePhysics(false);
	}
	
	Objects.Add(Object); 
}

void UObjectController::MoveObjectFromROS(AStaticMeshActor *Object, const mujoco_msgs::ModelState &ModelState)
{
	Object->SetActorTransform(FConversions::ROSToU(GetTransform(ModelState)));
}

void UObjectController::MoveObjectFromROS(AStaticMeshActor *Object, const visualization_msgs::Marker &ModelMarker)
{
	Object->SetActorTransform(FConversions::ROSToU(GetTransform(ModelMarker)));
}

void UObjectController::AddObjectFromROS(const mujoco_msgs::ModelState &ModelState)
{
	AsyncTask(ENamedThreads::GameThread, [ModelState, this]()
						{  
							FString ObjectName = ModelState.GetName();
							const FTransform Transform = GetTransform(ModelState);

							UE_LOG(LogObjectController, Log, TEXT("Try to add %s"), *ObjectName)
							FActorSpawnParameters SpawnParameters;
							SpawnParameters.Name = FName(ObjectName);
							AStaticMeshActor *Object = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FConversions::ROSToU(Transform), SpawnParameters);
							UStaticMeshComponent *StaticMeshComponent = Object->GetStaticMeshComponent();

							StaticMeshComponent->UnregisterComponent();

							FString MeshName;
							switch (ModelState.GetType())
							{
							case mujoco_msgs::ModelState::EType::CUBE:
								MeshName = TEXT("SM_Cube");
								break;

							case mujoco_msgs::ModelState::EType::SPHERE:
								MeshName = TEXT("SM_Sphere");
								break;

							case mujoco_msgs::ModelState::EType::CYLINDER:
								MeshName = TEXT("SM_Cylinder");
								break;

							case mujoco_msgs::ModelState::EType::MESH:
								MeshName = TEXT("SM_") + ObjectName;
								break;

							default:
								break;
							}

							UStaticMesh *StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *(TEXT("StaticMesh'/Game/Assets/StaticMeshes/") + MeshName + TEXT(".") + MeshName + TEXT("'"))));
							if (StaticMesh != nullptr)
							{
								if (StaticMeshComponent->SetStaticMesh(StaticMesh))
								{
									if (ModelState.GetType() != mujoco_msgs::ModelState::EType::MESH)
									{
										if (ColorMap.Find(ModelState.GetColor().GetColor()))
										{
											FString ColorName = TEXT("M_") + ColorMap[ModelState.GetColor().GetColor()];
											UMaterial *Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *(TEXT("StaticMesh'/Game/Assets/Materials/") + ColorName + TEXT(".") + ColorName + TEXT("'"))));
											StaticMeshComponent->SetMaterial(0, Material);
										}
										UE_LOG(LogObjectController, Log, TEXT("Added %s - [%f, %f, %f, %f]"), *StaticMeshComponent->GetStaticMesh()->GetName(), ModelState.GetColor().GetColor().X, ModelState.GetColor().GetColor().Y, ModelState.GetColor().GetColor().Z, ModelState.GetColor().GetColor().W)
									}
								}
								else
								{
									UE_LOG(LogObjectController, Error, TEXT("Failed to add %s"), *ObjectName)
								}
							}
							StaticMeshComponent->SetMobility(EComponentMobility::Movable);
							StaticMeshComponent->SetSimulatePhysics(false);
							StaticMeshComponent->RegisterComponent();
							Objects.Add(Object); });
}

bool UObjectController::AddOrMoveObjectFromROS(const mujoco_msgs::ModelState &ModelState)
{
	const FString ObjectName = ModelState.GetName();
	AStaticMeshActor *Object = GetObject(ObjectName);
	if (Object != nullptr)
	{
		MoveObjectFromROS(Object, ModelState);
		return true;
	}

	AddObjectFromROS(ModelState);
	return true;
}