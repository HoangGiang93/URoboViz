// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "Sensors/Sensor.h"
#include "Animation/SkeletalMeshActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogSensor, Log, All);

USensor::USensor()
{
}

void USensor::Init(ASkeletalMeshActor *InOwner)
{
  if (InOwner == nullptr)
  {
    UE_LOG(LogSensor, Error, TEXT("Owner of %s is nullptr"), *GetName())
    return;
  }

  SetOwner(InOwner);

  Init();
}
