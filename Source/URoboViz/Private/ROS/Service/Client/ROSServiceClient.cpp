// Fill out your copyright notice in the Description page of Project Settings.

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