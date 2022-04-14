// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "RoboManagerContainer.generated.h"

USTRUCT(Blueprintable)
struct FRoboManagerContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TMap<FString, class URobotController *> RobotControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TMap<FString, class USensor *> Sensors;

	FRoboManagerContainer()
	{
	}

	void Init(class ASkeletalMeshActor *Owner);

	void Tick(float DeltaTime);
};