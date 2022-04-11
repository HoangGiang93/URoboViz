// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectController.generated.h"

namespace mujoco_msgs
{
	class ObjectState;
	class ObjectStatus;
}

namespace visualization_msgs
{
	class Marker;
}

class AStaticMeshActor;

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOVIZ_API UObjectController : public UObject
{
	GENERATED_BODY()

public:
	UObjectController();

public:
	AStaticMeshActor *GetObjectInMujoco(const FString &ObjectName) const;

	AStaticMeshActor *GetObjectInUnreal(const FString &ObjectName) const;

	void AddObjectInMujoco(AStaticMeshActor *const ObjectStaticMesh);

	void RemoveObjectInMujoco(AStaticMeshActor *const ObjectStaticMesh);

	void SpawnObjectInUnreal(const mujoco_msgs::ObjectStatus &ObjectStatus);

  bool SpawnOrMoveObjectByMujoco(const mujoco_msgs::ObjectStatus &ObjectStatus);

	void MoveObjectByMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectStatus &ObjectStatus);

	void MoveObjectByMarker(AStaticMeshActor *Object, const visualization_msgs::Marker &ObjectMarker);

	void DestroyObjectInMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectState &ObjectState);

private:
	UPROPERTY(VisibleAnywhere)
	TSet<AStaticMeshActor *> ObjectsInMujoco;

	UPROPERTY(VisibleAnywhere)
	TSet<AStaticMeshActor *> ObjectsInUnreal;

private:
	TMap<FVector4, FString> ColorMap;
};