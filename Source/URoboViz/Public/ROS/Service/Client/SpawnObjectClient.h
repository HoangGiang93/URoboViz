// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSServiceClient.h"
// clang-format off
#include "SpawnObjectClient.generated.h"
// clang-format on

UCLASS()
class UROBOVIZ_API USpawnObjectClient : public UROSServiceClient
{
  GENERATED_BODY()

public:
  USpawnObjectClient();

protected:
  void Init() override;

  void CreateServiceClient() override;

public:
  void CallService(const TSet<class AStaticMeshActor *> &Objects);

private:
  UFUNCTION()
  void OnBeginOverlap(AActor *OverlappedActor, AActor *OtherActor);

public:
  UPROPERTY(EditAnywhere)
  FString FrameId = TEXT("");

  UPROPERTY(EditAnywhere)
  TSet<class ATriggerBase *> TriggerBases;

private:
  int32 Seq = 0;
};

class FSpawnObjectClientCallback : public FROSBridgeSrvClient
{
public:
  FSpawnObjectClientCallback(const FString &InServiceName, const FString &InServiceType);

  void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override;
};