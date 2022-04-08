// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboManager.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#include "ObjectController.h"
#include "mujoco_msgs/ModelState.h"

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
		Robot.Value.Tick();
	}

	ROSManager.Tick();
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