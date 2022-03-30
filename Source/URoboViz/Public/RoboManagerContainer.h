// Fill out your copyright notice in the Description page of Project Settings.

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