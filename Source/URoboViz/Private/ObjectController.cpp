// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ObjectController.h"
#include "Animation/SkeletalMeshActor.h"
#include "Async/Async.h"
#include "Controllers/JointController.h"
#include "Conversions.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/Paths.h"
#include "RoboAnim.h"
#include "mujoco_msgs/ObjectState.h"
#include "mujoco_msgs/ObjectStatus.h"
#include "visualization_msgs/Marker.h"

DEFINE_LOG_CATEGORY_STATIC(LogObjectController, Log, All);

static FTransform GetTransform(const mujoco_msgs::ObjectStatus &ObjectStatus)
{
	const FVector Location = ObjectStatus.GetPose().GetPosition().GetVector();
	const FRotator Rotation = ObjectStatus.GetPose().GetOrientation().GetQuat().Rotator();
	const FVector Scale = ObjectStatus.GetInfo().GetSize().Equals(FVector(0.f)) ? FVector(1.f) : ObjectStatus.GetInfo().GetSize();
	return FTransform(Rotation, Location, Scale);
}

static FTransform GetTransform(const visualization_msgs::Marker &ObjectMarker)
{
	const FVector Location = ObjectMarker.GetPose().GetPosition().GetVector();
	const FRotator Rotation = ObjectMarker.GetPose().GetOrientation().GetQuat().Rotator();
	const FVector Scale = ObjectMarker.GetScale().GetVector().Equals(FVector(0.f)) ? FVector(1.f) : ObjectMarker.GetScale().GetVector();
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
	for (AActor *const &Object : ObjectsInUnreal)
	{
		if (Object == nullptr)
		{
			continue;
		}

		if (ASkeletalMeshActor *SkeletalMeshObject = Cast<ASkeletalMeshActor>(Object))
		{
			if (USkeletalMeshComponent *SkeletalMeshComponent = SkeletalMeshObject->GetSkeletalMeshComponent())
			{
				URoboAnim *RoboAnim = Cast<URoboAnim>(SkeletalMeshComponent->GetAnimInstance());
				if (RoboAnim == nullptr)
				{
					UE_LOG(LogObjectController, Error, TEXT("Owner of %s does not have URoboAnim"), *GetName())
					return;
				}

				for (const TPair<FString, FJoint> &DesiredObjectJointPosition : DesiredObjectJointPositions[SkeletalMeshObject->GetName()])
				{
					if (RoboAnim->JointPositions.Contains(DesiredObjectJointPosition.Value.BoneName))
					{
						RoboAnim->JointPositions[DesiredObjectJointPosition.Value.BoneName] = DesiredObjectJointPosition.Value.JointPosition;
					}
				}
			}
		}

		const FVector LineStart = Object->GetActorLocation();
		if (bShowLinearVelocity)
		{
			if (AStaticMeshActor *StaticMeshObject = Cast<AStaticMeshActor>(Object))
			{
				if (StaticMeshObject->GetStaticMeshComponent() == nullptr)
				{
					UE_LOG(LogObjectController, Error, TEXT("%s has no StaticMeshComponent"), *StaticMeshObject->GetName())
					return;
				}

				FVector LinearVelocity = StaticMeshObject->GetStaticMeshComponent()->GetPhysicsLinearVelocity();
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
				DrawDebugString(GetWorld(), TextLocation, LinearVelocityText, StaticMeshObject, FColor::Blue, DeltaTime, false, 0.5f);
			}
		}
		if (bShowAngularVelocity)
		{
			if (AStaticMeshActor *StaticMeshObject = Cast<AStaticMeshActor>(Object))
			{
				if (StaticMeshObject->GetStaticMeshComponent() == nullptr)
				{
					UE_LOG(LogObjectController, Error, TEXT("%s has no StaticMeshComponent"), *StaticMeshObject->GetName())
					return;
				}

				FVector AngularVelocity = StaticMeshObject->GetStaticMeshComponent()->GetPhysicsAngularVelocityInDegrees();
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
}

AActor *UObjectController::GetObjectInMujoco(const FString &ObjectName) const
{
	for (AActor *const &Object : ObjectsInMujoco)
	{
		if (Object != nullptr && Object->GetName().Compare(ObjectName, ESearchCase::IgnoreCase) == 0)
		{
			return Object;
		}
	}
	UE_LOG(LogObjectController, Warning, TEXT("%s not found, try to capitalize first letter"), *ObjectName)
	return nullptr;
}

AActor *UObjectController::GetObjectInUnreal(const FString &ObjectName) const
{
	for (AActor *const &Object : ObjectsInUnreal)
	{
		if (Object != nullptr && Object->GetName().Compare(ObjectName) == 0)
		{
			return Object;
		}
	}
	return nullptr;
}

void UObjectController::AddObjectsInMujoco(const TSet<AActor *> &Objects)
{
	ObjectsInMujoco = ObjectsInMujoco.Union(Objects);
}

void UObjectController::RemoveObjectInMujoco(const TSet<AActor *> &Objects)
{
	for (AActor *const &Object : Objects)
	{
		ObjectsInMujoco.Remove(Object);
	}
}

void UObjectController::MoveObjectByMujoco(AActor *Object, const mujoco_msgs::ObjectStatus &ObjectStatus)
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

void UObjectController::MoveObjectByMujoco(AActor *Object, const mujoco_msgs::ObjectState &ObjectState)
{
	if (Object != nullptr)
	{
		const FVector Location = FConversions::ROSToU(ObjectState.GetPose().GetPosition().GetVector());
		const FQuat Rotation = FConversions::ROSToU(ObjectState.GetPose().GetOrientation().GetQuat());
		if (!Object->GetActorLocation().Equals(Location) || !Object->GetActorRotation().Equals(Rotation.Rotator()))
		{
			Object->SetActorLocationAndRotation(Location, Rotation);
		}
	}
	if (AStaticMeshActor *StaticMeshObject = Cast<AStaticMeshActor>(Object))
	{
		UStaticMeshComponent *StaticMeshComponent = StaticMeshObject->GetStaticMeshComponent();
		if (StaticMeshComponent->Mobility == EComponentMobility::Static)
		{
			return;
		}
		StaticMeshComponent->SetPhysicsLinearVelocity(FConversions::ROSToU(ObjectState.GetVelocity().GetLinear().GetVector()));
		StaticMeshComponent->SetPhysicsAngularVelocityInRadians(StaticMeshObject->GetActorRotation().RotateVector(ObjectState.GetVelocity().GetAngular().GetVector()) * FVector(-1, 1, -1));
	}
}

void UObjectController::MoveObjectByMarker(AActor *Object, const visualization_msgs::Marker &ObjectMarker)
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

							FActorSpawnParameters SpawnParameters;
							SpawnParameters.Name = FName(ObjectName);

							FString MeshPath;
							FString MeshTypeString;

							UClass *ActorClass = AStaticMeshActor::StaticClass();
							UStaticMesh *StaticMesh = nullptr;
							USkeletalMesh *SkeletalMesh = nullptr;

							if (ObjectStatus.GetInfo().GetType() == mujoco_msgs::ObjectInfo::MESH)
							{
								MeshTypeString = TEXT("Mesh");
								FString StaticMeshPath = TEXT("StaticMesh'") + ObjectStatus.GetInfo().GetMesh() + TEXT("'");
								StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *StaticMeshPath));

								FString SkeletalMeshPath = TEXT("SkeletalMesh'") + ObjectStatus.GetInfo().GetMesh() + TEXT("'");
								SkeletalMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *SkeletalMeshPath));

								if (StaticMesh != nullptr)
								{
									ActorClass = AStaticMeshActor::StaticClass();
									MeshPath = StaticMeshPath;
								}
								else if (SkeletalMesh != nullptr)
								{									
									ActorClass = ASkeletalMeshActor::StaticClass();
									MeshPath = SkeletalMeshPath;
								}
								else
								{
									ActorClass = nullptr;
									UE_LOG(LogObjectController, Warning, TEXT("File %s does not exist"), *ObjectStatus.GetInfo().GetMesh())
									return;
								}
							}

							AActor *Object = nullptr;
							if (ActorClass == AStaticMeshActor::StaticClass())
							{
								Object = GetWorld()->SpawnActor<AStaticMeshActor>(ActorClass, FConversions::ROSToU(Transform), SpawnParameters);
								UStaticMeshComponent *StaticMeshComponent = Cast<AStaticMeshActor>(Object)->GetStaticMeshComponent();

								StaticMeshComponent->UnregisterComponent();

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

								default:
									break;
								}

								if (StaticMesh != nullptr)
								{
									StaticMeshComponent->SetStaticMesh(StaticMesh);
									if (ObjectStatus.GetInfo().GetType() != mujoco_msgs::ObjectInfo::MESH)
									{
										const FVector4 Color = ObjectStatus.GetInfo().GetColor().GetColor();
										if (!Color.Equals(FVector4()) && ColorMap.Find(FLinearColor(Color)))
										{
											FString ColorName = TEXT("M_") + ColorMap[FLinearColor(Color)];
											UMaterial *Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *(TEXT("StaticMesh'/URoboViz/Assets/Materials/") + ColorName + TEXT(".") + ColorName + TEXT("'"))));
											for (int32 i = 0; i < StaticMeshComponent->GetMaterials().Num(); i++)
											{
												StaticMeshComponent->SetMaterial(i, Material);
											}
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
							}
							else if (ActorClass == ASkeletalMeshActor::StaticClass())
							{
								Object = GetWorld()->SpawnActor<ASkeletalMeshActor>(ActorClass, FConversions::ROSToU(Transform), SpawnParameters);
								USkeletalMeshComponent *SkeletalMeshComponent = Cast<ASkeletalMeshActor>(Object)->GetSkeletalMeshComponent();

								SkeletalMeshComponent->UnregisterComponent();

								if (SkeletalMesh != nullptr)
								{
									SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
									const FVector4 Color = ObjectStatus.GetInfo().GetColor().GetColor();
									if (!Color.Equals(FVector4()) && ColorMap.Find(FLinearColor(Color)))
									{
										FString ColorName = TEXT("M_") + ColorMap[FLinearColor(ObjectStatus.GetInfo().GetColor().GetColor())];
										UMaterial *Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *(TEXT("StaticMesh'/URoboViz/Assets/Materials/") + ColorName + TEXT(".") + ColorName + TEXT("'"))));
										for (int32 i = 0; i < SkeletalMeshComponent->GetMaterials().Num(); i++)
										{
											SkeletalMeshComponent->SetMaterial(i, Material);
										}
									}
								}

								if (RoboAnims.Contains(MeshPath))
								{
									SkeletalMeshComponent->SetAnimInstanceClass(RoboAnims[MeshPath]);
								}
								else
								{
									UE_LOG(LogObjectController, Warning, TEXT("RoboAnim of %s not found"), *MeshPath)
								}

								SkeletalMeshComponent->RegisterComponent();

								if (URoboAnim *RoboAnim = Cast<URoboAnim>(SkeletalMeshComponent->GetAnimInstance()))
								{
									TArray<FName> BoneNames;
									SkeletalMeshComponent->GetBoneNames(BoneNames);
									TMap<FString, EJointType> BoneTails = {{TEXT("_continuous_bone"), EJointType::Continuous}, {TEXT("_prismatic_bone"), EJointType::Prismatic}, {TEXT("_revolute_bone"), EJointType::Revolute}};
									DesiredObjectJointPositions.Add(ObjectName);
									for (const FName &BoneName : BoneNames)
									{
										for (const TPair<FString, EJointType> &BoneTail : BoneTails)
										{
											if (BoneName.ToString().Contains(BoneTail.Key, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
											{
												FString JointName = BoneName.ToString();
												JointName.RemoveFromEnd(BoneTail.Key);
												DesiredObjectJointPositions[ObjectName].Add(JointName, FJoint(BoneName, BoneTail.Value));
											}
										}
									}
									
									RoboAnim->NativeBeginPlay();
								}
							}

#if WITH_EDITOR
								Object->SetActorLabel(ObjectName);
#endif
								Object->Tags.Add(*MeshTypeString); 
									
								ObjectsInUnreal.Add(Object); 
								
								if (bAddObjectInMujoco)
								{
									ObjectsInMujoco.Add(Object);
								} });
}

bool UObjectController::SpawnOrMoveObjectByMujoco(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco)
{
	if (GetObjectInUnreal(ObjectStatus.GetInfo().GetName()) == nullptr)
	{
		SpawnObjectInUnreal(ObjectStatus, bAddObjectInMujoco);
		return true;
	}
	else if (AActor *Object = GetObjectInMujoco(ObjectStatus.GetInfo().GetName()))
	{
		MoveObjectByMujoco(Object, ObjectStatus);
		return true;
	}
	return false;
}

void UObjectController::SetDesiredObjectJointPositionFromROS(const FString &Objectname, const FString &JointName, const float DesiredObjectJointPosition)
{
	if (DesiredObjectJointPositions.Contains(Objectname) && DesiredObjectJointPositions[Objectname].Contains(JointName))
	{
		DesiredObjectJointPositions[Objectname][JointName].SetDesiredJointPositionFromROS(DesiredObjectJointPosition);
	}
}