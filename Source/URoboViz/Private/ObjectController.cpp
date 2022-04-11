// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectController.h"
#include "Async/Async.h"
#include "Conversions.h"
#include "Engine/StaticMeshActor.h"
#include "mujoco_msgs/ObjectState.h"
#include "mujoco_msgs/ObjectStatus.h"
#include "visualization_msgs/Marker.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectController, Log, All);

static FTransform GetTransform(const mujoco_msgs::ObjectStatus &ObjectStatus)
{
	const FVector Location = ObjectStatus.GetPose().GetPosition().GetVector();
	const FRotator Rotation = ObjectStatus.GetPose().GetOrientation().GetQuat().Rotator();
	const FVector Scale = ObjectStatus.GetInfo().GetSize();
	return FTransform(Rotation, Location, Scale);
}

static FTransform GetTransform(const visualization_msgs::Marker &ObjectMarker)
{
	const FVector Location = ObjectMarker.GetPose().GetPosition().GetVector();
	const FRotator Rotation = ObjectMarker.GetPose().GetOrientation().GetQuat().Rotator();
	const FVector Scale = ObjectMarker.GetScale().GetVector();
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

AStaticMeshActor *UObjectController::GetObjectInMujoco(const FString &ObjectName) const
{
	for (AStaticMeshActor *const &Object : ObjectsInMujoco)
	{
		if (Object->GetName().Compare(ObjectName) == 0)
		{
			return Object;
		}
	}
	return nullptr;
}

AStaticMeshActor *UObjectController::GetObjectInUnreal(const FString &ObjectName) const
{
	for (AStaticMeshActor *const &Object : ObjectsInUnreal)
	{
		if (Object->GetName().Compare(ObjectName) == 0)
		{
			return Object;
		}
	}
	return nullptr;
}

void UObjectController::AddObjectInMujoco(AStaticMeshActor *const Object)
{
	Object->GetStaticMeshComponent()->SetSimulatePhysics(false);
	ObjectsInMujoco.Add(Object);
}

void UObjectController::RemoveObjectInMujoco(AStaticMeshActor *const Object)
{
	Object->GetStaticMeshComponent()->SetSimulatePhysics(true);
	ObjectsInMujoco.Remove(Object);
}

void UObjectController::MoveObjectByMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectStatus &ObjectStatus)
{
	if (Object != nullptr)
	{
		Object->SetActorTransform(FConversions::ROSToU(GetTransform(ObjectStatus)));
	}
	else
	{
		UE_LOG(LogObjectController, Error, TEXT("Object is nullptr"))
	}
}

void UObjectController::MoveObjectByMarker(AStaticMeshActor *Object, const visualization_msgs::Marker &ObjectMarker)
{
	if (Object != nullptr)
	{
		Object->SetActorTransform(FConversions::ROSToU(GetTransform(ObjectMarker)));
	}
	else
	{
		UE_LOG(LogObjectController, Error, TEXT("Object is nullptr"))
	}
}

void UObjectController::SpawnObjectInUnreal(const mujoco_msgs::ObjectStatus &ObjectStatus)
{
	AsyncTask(ENamedThreads::GameThread, [ObjectStatus, this]()
						{  
							FString ObjectName = ObjectStatus.GetInfo().GetName();
							const FTransform Transform = GetTransform(ObjectStatus);

							UE_LOG(LogObjectController, Log, TEXT("Try to add %s"), *ObjectName)
							FActorSpawnParameters SpawnParameters;
							SpawnParameters.Name = FName(ObjectName);
							AStaticMeshActor *Object = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FConversions::ROSToU(Transform), SpawnParameters);
							UStaticMeshComponent *StaticMeshComponent = Object->GetStaticMeshComponent();

							StaticMeshComponent->UnregisterComponent();

							FString MeshPath;
							switch (ObjectStatus.GetInfo().GetType())
							{
							case mujoco_msgs::ObjectInfo::CUBE:
								MeshPath = TEXT("StaticMesh'/Game/Assets/StaticMeshes/SM_Cube.SM_Cube'");
								break;

							case mujoco_msgs::ObjectInfo::SPHERE:
								MeshPath = TEXT("StaticMesh'/Game/Assets/StaticMeshes/SM_Sphere.SM_Sphere'");
								break;

							case mujoco_msgs::ObjectInfo::CYLINDER:
								MeshPath = TEXT("StaticMesh'/Game/Assets/StaticMeshes/SM_Cylinder.SM_Cylinder'");
								break;

							case mujoco_msgs::ObjectInfo::MESH:
								MeshPath = TEXT("StaticMesh'/Game/") + ObjectStatus.GetInfo().GetMesh() + TEXT("'");
								break;

							default:
								break;
							}

							UStaticMesh *StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
							if (StaticMesh != nullptr)
							{
								StaticMeshComponent->SetStaticMesh(StaticMesh);
								if (ObjectStatus.GetInfo().GetType() != mujoco_msgs::ObjectInfo::MESH)
								{
									if (ColorMap.Find(ObjectStatus.GetInfo().GetColor().GetColor()))
									{
										FString ColorName = TEXT("M_") + ColorMap[ObjectStatus.GetInfo().GetColor().GetColor()];
										UMaterial *Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *(TEXT("StaticMesh'/Game/Assets/Materials/") + ColorName + TEXT(".") + ColorName + TEXT("'"))));
										StaticMeshComponent->SetMaterial(0, Material);
									}
									UE_LOG(LogObjectController, Log, TEXT("Added %s - [%f, %f, %f, %f]"), 
												*StaticMeshComponent->GetStaticMesh()->GetName(), 
												ObjectStatus.GetInfo().GetColor().GetColor().X, 
												ObjectStatus.GetInfo().GetColor().GetColor().Y, 
												ObjectStatus.GetInfo().GetColor().GetColor().Z, 
												ObjectStatus.GetInfo().GetColor().GetColor().W)
								}
							}
							StaticMeshComponent->SetMobility(EComponentMobility::Movable);
							StaticMeshComponent->SetSimulatePhysics(true);
							StaticMeshComponent->SetGenerateOverlapEvents(true);
							StaticMeshComponent->RegisterComponent();

							Object->Tags.Add(TEXT("tf")); 
							
							ObjectsInUnreal.Add(Object); });
}

bool UObjectController::SpawnOrMoveObjectByMujoco(const mujoco_msgs::ObjectStatus &ObjectStatus)
{
	if (GetObjectInUnreal(ObjectStatus.GetInfo().GetName()) == nullptr)
	{
		SpawnObjectInUnreal(ObjectStatus);
		return true;
	}
	else if (AStaticMeshActor *Object = GetObjectInMujoco(ObjectStatus.GetInfo().GetName()))
	{
		MoveObjectByMujoco(Object, ObjectStatus);
		return true;
	}
	return false;
}

void UObjectController::DestroyObjectInMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectState &ObjectState)
{
	if (Object != nullptr && Object->GetStaticMeshComponent() != nullptr)
	{
		UStaticMeshComponent *StaticMeshComponent = Object->GetStaticMeshComponent();
		StaticMeshComponent->SetPhysicsLinearVelocity(FConversions::ROSToU(ObjectState.GetVelocity().GetLinear().GetVector()));
		StaticMeshComponent->SetPhysicsAngularVelocityInRadians(ObjectState.GetVelocity().GetAngular().GetVector() * FVector(-1, 1, -1));
	}
	RemoveObjectInMujoco(Object);
}