// Fill out your copyright notice in the Description page of Project Settings.

#include "ROS/ROSBase.h"
#include "RoboManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSBase, Log, All);

void UROSBase::Connect(const FString &Host, const int32 Port)
{
  Handler = MakeShareable<FROSBridgeHandler>(new FROSBridgeHandler(Host, Port));

  if (!Handler.IsValid())
  {
    return;
  }
  Handler->Connect();

  Init();

  if (ARoboManager *Outer = Cast<ARoboManager>(GetOuter()))
  {
    RoboManager = Outer;
  }
  else
  {
    UE_LOG(LogROSBase, Error, TEXT("Outer of %s is not ARoboManager"), *GetName())
  }
}

void UROSBase::Init()
{
  if (ARoboManager *Outer = Cast<ARoboManager>(GetOuter()))
  {
    RoboManager = Outer;
  }
  else
  {
    UE_LOG(LogROSBase, Error, TEXT("Outer of %s is not ARoboManager"), *GetName())
  }
}

void UROSBase::Disconnect()
{
	if (Handler.IsValid())
	{
		Handler->Disconnect();
	}
}

void UROSBase::Tick()
{
	if (Handler.IsValid())
	{
		Handler->Process();
	}
}