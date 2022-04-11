// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "Controllers/BaseController.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseController, Log, All);

UBaseController::UBaseController()
{
}

void UBaseController::Tick()
{
  if (GetOwner() == nullptr)
  {
    UE_LOG(LogBaseController, Error, TEXT("Owner of %s is nullptr"), *GetName())
    return;
  }

  GetOwner()->SetActorLocationAndRotation(BaseLocation, BaseRotation);
}

void UBaseController::SetDesiredBasePoseFromROS(const FVector &InBaseLocation, const FQuat &InBaseRotation)
{
  BaseLocation = FConversions::ROSToU(InBaseLocation);
  BaseRotation = FConversions::ROSToU(InBaseRotation);
}