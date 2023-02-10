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

class AActor;

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOVIZ_API UObjectController : public UObject
{
	GENERATED_BODY()

public:
	UObjectController();

public:
	void Tick(float DeltaTime);

	AActor *GetObjectInMujoco(const FString &ObjectName) const;

	AActor *GetObjectInUnreal(const FString &ObjectName) const;

	TSet<AActor *> GetObjectsInMujoco() const { return ObjectsInMujoco; }

	TSet<AActor *> GetObjectsInUnreal() const { return ObjectsInUnreal; }

	void AddObjectsInMujoco(const TSet<AActor *> &Objects);

	void RemoveObjectInMujoco(const TSet<AActor *> &Objects);

	void SpawnObjectInUnreal(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco = false);

	bool SpawnOrMoveObjectByMujoco(const mujoco_msgs::ObjectStatus &ObjectStatus, const bool bAddObjectInMujoco = false);

	void MoveObjectByMujoco(AActor *Object, const mujoco_msgs::ObjectStatus &ObjectStatus);

	void MoveObjectByMujoco(AActor *Object, const mujoco_msgs::ObjectState &ObjectState);

	void MoveObjectByMarker(AActor *Object, const visualization_msgs::Marker &ObjectMarker);

	void SetDesiredObjectJointPositionFromROS(const FString &Objectname, const FString &JointName, const float DesiredObjectJointPosition);

public:
	UPROPERTY(EditAnywhere)
	bool bShowLinearVelocity;

	UPROPERTY(EditAnywhere)
	bool bShowAngularVelocity;

private:
	UPROPERTY(VisibleAnywhere)
	TSet<AActor *> ObjectsInMujoco;

	UPROPERTY(EditAnywhere)
	TSet<AActor *> ObjectsInUnreal;

	UPROPERTY(EditAnywhere)
	TMap<FString, TSubclassOf<UAnimInstance>> RoboAnims;

private:
	TMap<FLinearColor, FString> ColorMap;

	TMap<FString, TMap<FString, struct FJoint>> DesiredObjectJointPositions;
};