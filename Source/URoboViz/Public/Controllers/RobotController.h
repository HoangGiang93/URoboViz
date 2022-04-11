// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "CoreMinimal.h"
// clang-format off
#include "RobotController.generated.h"
// clang-format on

class ASkeletalMeshActor;

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOVIZ_API URobotController : public UObject
{
	GENERATED_BODY()

public:
	URobotController();

public:
	void Init(ASkeletalMeshActor *InOwner);

	void SetOwner(ASkeletalMeshActor *InOwner) { Owner = InOwner; }

	ASkeletalMeshActor *GetOwner() const { return Owner; }

public:
	virtual void Tick() {}

protected:
	virtual void Init() {}

private:
	ASkeletalMeshActor *Owner;
};
