// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

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
	void Tick(float DeltaTime);

	AStaticMeshActor *GetObjectInMujoco(const FString &ObjectName) const;

	AStaticMeshActor *GetObjectInUnreal(const FString &ObjectName) const;

	TSet<AStaticMeshActor *> GetObjectsInMujoco() const { return ObjectsInMujoco; }

	TSet<AStaticMeshActor *> GetObjectsInUnreal() const { return ObjectsInUnreal; }

	void AddObjectsInMujoco(const TSet<AStaticMeshActor *> &Objects);

	void RemoveObjectInMujoco(const TSet<AStaticMeshActor *> &Objects);

	void SpawnObjectInUnreal(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco = false);

	bool SpawnOrMoveObjectByMujoco(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco = false);

	void MoveObjectByMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectStatus &ObjectStatus);

	void MoveObjectByMujoco(AStaticMeshActor *Object, const mujoco_msgs::ObjectState &ObjectState);

	void MoveObjectByMarker(AStaticMeshActor *Object, const visualization_msgs::Marker &ObjectMarker);

public:
	UPROPERTY(EditAnywhere)
	bool bShowLinearVelocity;

	UPROPERTY(EditAnywhere)
	bool bShowAngularVelocity;

private:
	UPROPERTY(VisibleAnywhere)
	TSet<AStaticMeshActor *> ObjectsInMujoco;

	UPROPERTY(EditAnywhere)
	TSet<AStaticMeshActor *> ObjectsInUnreal;

private:
	TMap<FLinearColor, FString> ColorMap;
};