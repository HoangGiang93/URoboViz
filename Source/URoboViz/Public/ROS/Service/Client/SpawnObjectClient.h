// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSServiceClient.h"
// clang-format off
#include "SpawnObjectClient.generated.h"
// clang-format on

class UObjectController;

UCLASS()
class UROBOVIZ_API USpawnObjectClient : public UROSServiceClient
{
  GENERATED_BODY()

public:
  USpawnObjectClient();

public:
  void Tick() override;

protected:
  void Init() override;

  void CreateServiceClient() override;

private:
  UFUNCTION()
  void OnBeginOverlap(AActor *OverlappedActor, AActor *OtherActor);

public:
  UPROPERTY(EditAnywhere)
  TSet<class ATriggerBase *> TriggerBases;

  UPROPERTY(EditAnywhere)
  int32 SpawnObjectCountPerCycle = 20;

private:
  int32 Seq = 0;
};

class FSpawnObjectClientCallback : public FROSBridgeSrvClient
{
public:
  FSpawnObjectClientCallback(const FString &InServiceName, const FString &InServiceType, UObjectController *InObjectController);

  void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override;

private:
  UObjectController *ObjectController;
};