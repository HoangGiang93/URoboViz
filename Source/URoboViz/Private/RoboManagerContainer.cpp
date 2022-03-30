// Fill out your copyright notice in the Description page of Project Settings.


#include "RoboManagerContainer.h"
#include "Controllers/RobotController.h"

void FRoboManagerContainer::Init(ASkeletalMeshActor *Owner)
{
	for (TPair<FString, URobotController *>RobotController : RobotControllers)
	{
		if (RobotController.Value == nullptr)
		{
			continue;
		}
		RobotController.Value->Rename(*RobotController.Key);
		RobotController.Value->Init(Owner);
	}
}

void FRoboManagerContainer::Tick()
{
	for (TPair<FString, URobotController *>RobotController : RobotControllers)
	{
		if (RobotController.Value == nullptr)
		{
			continue;
		}

		RobotController.Value->Tick();
	}
}