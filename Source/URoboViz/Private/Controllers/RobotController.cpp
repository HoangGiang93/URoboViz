// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "Controllers/RobotController.h"
#include "Animation/SkeletalMeshActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogRobotController, Log, All);

URobotController::URobotController()
{
}

void URobotController::Init(ASkeletalMeshActor *InOwner)
{
  if (InOwner == nullptr)
  {
    UE_LOG(LogRobotController, Error, TEXT("Owner of %s is nullptr"), *GetName())
    return;
  }

  SetOwner(InOwner);

  Init();
}
