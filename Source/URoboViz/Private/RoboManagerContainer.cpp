// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "RoboManagerContainer.h"
#include "Controllers/RobotController.h"
#include "Sensors/Sensor.h"

void FRoboManagerContainer::Init(ASkeletalMeshActor *Owner)
{
	for (TPair<FString, URobotController *> RobotController : RobotControllers)
	{
		if (RobotController.Value == nullptr)
		{
			continue;
		}
		RobotController.Value->Rename(*RobotController.Key);
		RobotController.Value->Init(Owner);
	}
	for (TPair<FString, USensor *> Sensor : Sensors)
	{
		if (Sensor.Value == nullptr)
		{
			continue;
		}
		Sensor.Value->Rename(*Sensor.Key);
		Sensor.Value->Init(Owner);
	}
}

void FRoboManagerContainer::Tick(float DeltaTime)
{
	for (TPair<FString, URobotController *> RobotController : RobotControllers)
	{
		if (RobotController.Value == nullptr)
		{
			continue;
		}

		RobotController.Value->Tick();
	}
	for (TPair<FString, USensor *> Sensor : Sensors)
	{
		if (Sensor.Value == nullptr)
		{
			continue;
		}

		Sensor.Value->Tick(DeltaTime);
	}
}