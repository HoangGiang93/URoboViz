// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

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

  void Connect(const TSharedPtr<FROSBridgeHandler> &InHandler);

  void Disconnect();

  virtual void Tick() {}

protected:
  virtual void Init();

public:
  ARoboManager *GetRoboManager() const { return RoboManager; }

protected:
  TSharedPtr<FROSBridgeHandler> Handler;

private:
  ARoboManager *RoboManager;
};