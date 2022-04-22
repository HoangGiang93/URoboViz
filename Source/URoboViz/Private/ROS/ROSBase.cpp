// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/ROSBase.h"
#include "RoboManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSBase, Log, All);

void UROSBase::Connect(const TSharedPtr<FROSBridgeHandler> &InHandler)
{
  Handler = InHandler;

  Init();
}

void UROSBase::Connect(const FString &Host, const int32 Port)
{
  Handler = MakeShareable<FROSBridgeHandler>(new FROSBridgeHandler(Host, Port));

  Handler->Connect();

  Init();
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