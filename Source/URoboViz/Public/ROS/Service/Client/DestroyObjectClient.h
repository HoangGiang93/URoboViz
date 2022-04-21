// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSServiceClient.h"
// clang-format off
#include "DestroyObjectClient.generated.h"
// clang-format on

class UObjectController;

UCLASS()
class UROBOVIZ_API UDestroyObjectClient : public UROSServiceClient
{
  GENERATED_BODY()

public:
  UDestroyObjectClient();

public:
  void Tick() override;

protected:
  void Init() override;

  void CreateServiceClient() override;

private:
  UFUNCTION()
  void OnEndOverlap(AActor *OverlappedActor, AActor *OtherActor);

public:
  UPROPERTY(EditAnywhere)
  TSet<class ATriggerBase *> TriggerBases;
};

class FDestroyObjectClientCallback : public FROSBridgeSrvClient
{
public:
  FDestroyObjectClientCallback(const FString &InServiceName, const FString &InServiceType, UObjectController *InObjectController);

  void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override;

private:
  UObjectController *ObjectController;
};