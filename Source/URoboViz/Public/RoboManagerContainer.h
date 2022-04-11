// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "RoboManagerContainer.generated.h"

class ASkeletalMeshActor;
class URobotController;

USTRUCT(Blueprintable)
struct FRoboManagerContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TMap<FString, URobotController *> RobotControllers;

	FRoboManagerContainer()
	{
	}

	void Init(ASkeletalMeshActor *Owner);

	void Tick();
};