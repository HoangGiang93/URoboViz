// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Subscriber/ROSSubscriber.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSSubscriber, Log, All)

void UROSSubscriber::Init()
{
  Super::Init();
  CreateSubscriber();
  if (Subscriber.IsValid())
  {
    Handler->AddSubscriber(Subscriber);
  }
  else
  {
    UE_LOG(LogROSSubscriber, Warning, TEXT("%s is not valid"), *GetName())
  }
}