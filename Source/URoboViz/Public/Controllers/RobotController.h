// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RobotController.generated.h"

/**
 * 
 */
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
