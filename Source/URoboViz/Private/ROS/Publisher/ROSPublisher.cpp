// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Publisher/ROSPublisher.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSPublisher, Log, All)

void UROSPublisher::Init()
{
  Super::Init();
  Publisher = MakeShareable<FROSBridgePublisher>(new FROSBridgePublisher(CommonPublisherParameters.Topic, CommonPublisherParameters.MessageType));
  if (Publisher.IsValid())
  {
    Handler->AddPublisher(Publisher);
  }
  else
  {
    UE_LOG(LogROSPublisher, Warning, TEXT("%s is not valid"), *GetName())
  }
  PublishTime = FROSTime::Now();
}

void UROSPublisher::Tick()
{
  if (Handler.IsValid())
  {
    if ((FROSTime::Now().GetTimeAsDouble() - PublishTime.GetTimeAsDouble()) > 1 / CommonPublisherParameters.PublishRate)
    {
      Publish();
      PublishTime = FROSTime::Now();
    }
  }
}
