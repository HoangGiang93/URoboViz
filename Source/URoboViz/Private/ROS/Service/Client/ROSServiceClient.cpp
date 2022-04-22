// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Service/Client/ROSServiceClient.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSServiceClient, Log, All)

void UROSServiceClient::Init()
{
  Super::Init();
  CreateServiceClient();
}

void UROSServiceClient::CallService()
{
  if (ServiceClient.IsValid())
  {
    Handler->CallService(ServiceClient, Request, Response);
  }
}

void UROSServiceClient::Tick()
{
  if (Handler.IsValid())
	{
		Handler->Process();
	}
}