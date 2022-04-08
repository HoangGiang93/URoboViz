// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ROSBridgeSrvClient.h"
#include "ROS/ROSBase.h"
// clang-format off
#include "ROSServiceClient.generated.h"
// clang-format on

USTRUCT()
struct FROSServiceClientParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString ServiceName;

  UPROPERTY(VisibleAnywhere)
  FString ServiceType;
};

UCLASS()
class UROBOVIZ_API UROSServiceClient : public UROSBase
{
  GENERATED_BODY()

public:
  void CallService();

public:
  virtual void Init() override;

protected:
  virtual void CreateServiceClient() {}

public:
  UPROPERTY(EditAnywhere)
  FROSServiceClientParameter CommonServiceClientParameters;

protected:
  TSharedPtr<FROSBridgeSrvClient> ServiceClient;

  TSharedPtr<FROSBridgeSrv::SrvRequest> Request;

  TSharedPtr<FROSBridgeSrv::SrvResponse> Response;
};