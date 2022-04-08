// Fill out your copyright notice in the Description page of Project Settings.

#include "ROS/Service/Server/ROSServiceServer.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSServiceServer, Log, All)

void UROSServiceServer::Init()
{
  Super::Init();
  CreateServiceServer();
  if (ServiceServer.IsValid())
  {
    Handler->AddServiceServer(ServiceServer);
  }
  else
  {
    UE_LOG(LogROSServiceServer, Warning, TEXT("%s is not valid"), *GetName())
  }
}

