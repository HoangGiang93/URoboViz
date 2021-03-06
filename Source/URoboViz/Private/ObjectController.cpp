// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ObjectController.h"
#include "Async/Async.h"
#include "Conversions.h"
#include "DrawDebugHelpers.h"
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
			{FLinearColor(0, 0, 1, 1), TEXT("Blue")},
			{FLinearColor(0, 1, 1, 1), TEXT("Cyan")},
			{FLinearColor(0, 1, 0, 1), TEXT("Green")},
			{FLinearColor(1, 0, 0.5, 1), TEXT("Pink")},
			{FLinearColor(0.5, 0, 1, 1), TEXT("Purple")},
			{FLinearColor(1, 0, 0, 1), TEXT("Red")},
			{FLinearColor(1, 1, 0, 1), TEXT("Yellow")},
			{FLinearColor(0.8, 0.1, 0, 1), TEXT("Orange")}};
}

void UObjectController::Tick(float DeltaTime)
{
	for (AStaticMeshActor *const &Object : ObjectsInUnreal)
	{
		if (Object == nullptr)
		{
			continue;
		}
		const FVector LineStart = Object->GetActorLocation();
		if (bShowLinearVelocity)
		{
			FVector LinearVelocity = Object->GetStaticMeshComponent()->GetPhysicsLinearVelocity();
			const double LinearVelocityLength = LinearVelocity.Size();
			LinearVelocity.Normalize();
			const FString LinearVelocityText = FString::Printf(TEXT("[%.2f, %.2f, %.2f] * %.2f m/s"), LinearVelocity.X, LinearVelocity.Y, LinearVelocity.Z, LinearVelocityLength / 100.f);
			const FVector LineEnd = LineStart + LinearVelocity * 10;
			DrawDebugDirectionalArrow(GetWorld(), LineStart, LineEnd, 1.f, FColor::Blue, false, -1.f, 0, 1.f);
			FVector TextLocation;
			if (LinearVelocityLength > 0.1f)
			{
				TextLocation = LinearVelocity * 10;
			}
			DrawDebugString(GetWorld(), TextLocation, LinearVelocityText, Object, FColor::Blue, DeltaTime, false, 0.5f);
		}
		if (bShowAngularVelocity)
		{
			FVector AngularVelocity = Object->GetStaticMeshComponent()->GetPhysicsAngularVelocityInDegrees();
			double AngularVelocityLength = AngularVelocity.Size();
			AngularVelocity.Normalize();
			FString AngularVelocityText = FString::Printf(TEXT("[%.2f, %.2f, %.2f] * %.2f deg/s"), AngularVelocity.X, AngularVelocity.Y, AngularVelocity.Z, AngularVelocityLength);
			const FVector LineEnd = LineStart + AngularVelocity * 10;
			DrawDebugDirectionalArrow(GetWorld(), LineStart, LineEnd, 1.f, FColor::Cyan, false, -1.f, 0, 1.f);
			FVector TextLocation;
			if (AngularVelocityLength > 1.f)
			{
				TextLocation = AngularVelocity * 10;
			}
			DrawDebugString(GetWorld(), TextLocation, AngularVelocityText, Object, FColor::Cyan, DeltaTime, false, 0.5f);
		}
	}
}

AStaticMeshActor *UObjectController::GetObjectInMujoco(const FString &ObjectName) const
{
	for (AStaticMeshActor *const &Object : ObjectsInMujoco)
	{
		if (Object != nullptr && Object->GetName().Compare(ObjectName) == 0)
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
		if (Object != nullptr && Object->GetName().Compare(ObjectName) == 0)
		{
			return Object;
		}
	}
	return nullptr;
}

void UObjectController::AddObjectsInMujoco(const TSet<AStaticMeshActor *> &Objects)
{
	ObjectsInMujoco = ObjectsInMujoco.Union(Objects);
}

void UObjectController::RemoveObjectInMujoco(const TSet<AStaticMeshActor *> &Objects)
{
	for (AStaticMeshActor *const &Object : Objects)
	{
		ObjectsInMujoco.Remove(Object);
	}
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

void UObjectController::MoveObjectByMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectState &ObjectState)
{
	if (Object != nullptr)
	{
		const FVector Location = FConversions::ROSToU(ObjectState.GetPose().GetPosition().GetVector());
		const FQuat Rotation = FConversions::ROSToU(ObjectState.GetPose().GetOrientation().GetQuat());
		if (!Object->GetActorLocation().Equals(Location) || !Object->GetActorRotation().Equals(Rotation.Rotator()))
		{
			Object->SetActorLocationAndRotation(Location, Rotation);
		}

		UStaticMeshComponent *StaticMeshComponent = Object->GetStaticMeshComponent();
		if (StaticMeshComponent->Mobility == EComponentMobility::Static)
		{
			return;
		}
		StaticMeshComponent->SetPhysicsLinearVelocity(FConversions::ROSToU(ObjectState.GetVelocity().GetLinear().GetVector()));
		StaticMeshComponent->SetPhysicsAngularVelocityInRadians(Object->GetActorRotation().RotateVector(ObjectState.GetVelocity().GetAngular().GetVector()) * FVector(-1, 1, -1));
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

void UObjectController::SpawnObjectInUnreal(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco)
{
	AsyncTask(ENamedThreads::GameThread, [ObjectStatus, bAddObjectInMujoco, this]()
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
							FString MeshTypeString;
							switch (ObjectStatus.GetInfo().GetType())
							{
							case mujoco_msgs::ObjectInfo::CUBE:
								MeshPath = TEXT("StaticMesh'/URoboViz/Assets/StaticMeshes/SM_Cube.SM_Cube'");
								MeshTypeString = TEXT("Cube");
								break;

							case mujoco_msgs::ObjectInfo::SPHERE:
								MeshPath = TEXT("StaticMesh'/URoboViz/Assets/StaticMeshes/SM_Sphere.SM_Sphere'");
								MeshTypeString = TEXT("Sphere");
								break;

							case mujoco_msgs::ObjectInfo::CYLINDER:
								MeshPath = TEXT("StaticMesh'/URoboViz/Assets/StaticMeshes/SM_Cylinder.SM_Cylinder'");
								MeshTypeString = TEXT("Cylinder");
								break;

							case mujoco_msgs::ObjectInfo::MESH:
								MeshPath = TEXT("StaticMesh'") + ObjectStatus.GetInfo().GetMesh() + TEXT("'");
								MeshTypeString = TEXT("Mesh");
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
									if (ColorMap.Find(FLinearColor(ObjectStatus.GetInfo().GetColor().GetColor())))
									{
										FString ColorName = TEXT("M_") + ColorMap[FLinearColor(ObjectStatus.GetInfo().GetColor().GetColor())];
										UMaterial *Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *(TEXT("StaticMesh'/URoboViz/Assets/Materials/") + ColorName + TEXT(".") + ColorName + TEXT("'"))));
										StaticMeshComponent->SetMaterial(0, Material);
									}
								}
							}
							if (ObjectStatus.GetInfo().GetMovable())
							{
								StaticMeshComponent->SetMobility(EComponentMobility::Movable);
								StaticMeshComponent->SetSimulatePhysics(!bAddObjectInMujoco);
							}
							else
							{
								StaticMeshComponent->SetMobility(EComponentMobility::Static);
								StaticMeshComponent->SetSimulatePhysics(false);
							}
							
							StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
							StaticMeshComponent->SetGenerateOverlapEvents(true);
							StaticMeshComponent->RegisterComponent();

							Object->Tags.Add(*MeshTypeString); 
							
							ObjectsInUnreal.Add(Object); 
							
							if (bAddObjectInMujoco)
							{
								ObjectsInMujoco.Add(Object);
							}
							});
}

bool UObjectController::SpawnOrMoveObjectByMujoco(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco)
{
	if (GetObjectInUnreal(ObjectStatus.GetInfo().GetName()) == nullptr)
	{
		SpawnObjectInUnreal(ObjectStatus, bAddObjectInMujoco);
		return true;
	}
	else if (AStaticMeshActor *Object = GetObjectInMujoco(ObjectStatus.GetInfo().GetName()))
	{
		MoveObjectByMujoco(Object, ObjectStatus);
		return true;
	}
	return false;
}