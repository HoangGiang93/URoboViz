// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "RoboManager.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#include "ObjectController.h"
#include "mujoco_msgs/ObjectState.h"

DEFINE_LOG_CATEGORY_STATIC(LogRoboManager, Log, All);

// Sets default values
ARoboManager::ARoboManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectController = CreateDefaultSubobject<UObjectController>(TEXT("ObjectController"));
}

// Called when the game starts or when spawned
void ARoboManager::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void ARoboManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ROSManager.Deinit();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ARoboManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TPair<ASkeletalMeshActor *, FRoboManagerContainer> Robot : RobotManager)
	{
		if (Robot.Key == nullptr)
		{
			continue;
		}
		Robot.Value.Tick(DeltaTime);
	}

	ROSManager.Tick();

	ObjectController->Tick(DeltaTime);
}

void ARoboManager::Init()
{
	for (TPair<ASkeletalMeshActor *, FRoboManagerContainer> Robot : RobotManager)
	{
		if (Robot.Key == nullptr)
		{
			continue;
		}
		Robot.Value.Init(Robot.Key);
	}

	ROSManager.Init();
}

URobotController *ARoboManager::GetController(const FString &ControllerName) const
{
	TArray<FRoboManagerContainer> RobotManagerContainers;
	RobotManager.GenerateValueArray(RobotManagerContainers);
	for (const FRoboManagerContainer &RobotManagerContainer : RobotManagerContainers)
	{
		if (RobotManagerContainer.RobotControllers.Find(ControllerName))
		{
			return RobotManagerContainer.RobotControllers[ControllerName];
		}
	}
	UE_LOG(LogRoboManager, Warning, TEXT("Controller %s not found"), *ControllerName)
	return nullptr;
}

USensor *ARoboManager::GetSensor(const FString &SensorName) const
{
	TArray<FRoboManagerContainer> RobotManagerContainers;
	RobotManager.GenerateValueArray(RobotManagerContainers);
	for (const FRoboManagerContainer &RobotManagerContainer : RobotManagerContainers)
	{
		if (RobotManagerContainer.Sensors.Find(SensorName))
		{
			return RobotManagerContainer.Sensors[SensorName];
		}
	}
	UE_LOG(LogRoboManager, Warning, TEXT("Sensor %s not found"), *SensorName)
	return nullptr;
}