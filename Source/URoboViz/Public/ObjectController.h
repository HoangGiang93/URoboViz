// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectController.generated.h"

namespace mujoco_msgs
{
	class ModelState;
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
	AStaticMeshActor *GetObject(const FString &ObjectName) const;

	void AddObject(AStaticMeshActor *const Object);

	void MoveObjectFromROS(AStaticMeshActor *Object, const mujoco_msgs::ModelState &ModelState);

	void MoveObjectFromROS(AStaticMeshActor *Object, const visualization_msgs::Marker &ModelState);

	void AddObjectFromROS(const mujoco_msgs::ModelState &ModelState);

  bool AddOrMoveObjectFromROS(const mujoco_msgs::ModelState &ModelState);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<AStaticMeshActor *> Objects;

private:
	TMap<FVector4, FString> ColorMap;
};