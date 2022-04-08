// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ROSBridgeHandler.h"
#include "RoboManager.h"
// clang-format off
#include "ROSBase.generated.h"
// clang-format on

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOVIZ_API UROSBase : public UObject
{
  GENERATED_BODY()

public:
  void Connect(const FString &Host, const int32 Port);

  void Disconnect();

  virtual void Tick();

protected:
  virtual void Init();

public:
  ARoboManager *GetRoboManager() const { return RoboManager; }

protected:
  TSharedPtr<FROSBridgeHandler> Handler;

private:
  ARoboManager *RoboManager;
};