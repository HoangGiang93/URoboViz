// Fill out your copyright notice in the Description page of Project Settings.

#include "ROS/ROSBase.h"
#include "RoboManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogROSBase, Log, All);

void UROSBase::Connect(const TSharedPtr<FROSBridgeHandler> &InHandler)
{
  if (!InHandler.IsValid())
  {
    UE_LOG(LogROSBase, Error, TEXT("No FROSBridgeHandler created in %s"), *GetName())
    return;
  }
  Handler = InHandler;
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