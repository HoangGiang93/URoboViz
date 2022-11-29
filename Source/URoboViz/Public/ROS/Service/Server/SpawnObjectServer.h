// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSServiceServer.h"
// clang-format off
#include "SpawnObjectServer.generated.h"
// clang-format on

class ObjectController;

UCLASS()
class UROBOVIZ_API USpawnObjectServer : public UROSServiceServer
{
  GENERATED_BODY()

public:
  USpawnObjectServer();

public:
  UPROPERTY(EditAnywhere)
  bool bAddObjectInMujoco;

protected:
  virtual void CreateServiceServer() override;
};

class FSpawnObjectServerCallback final : public FROSBridgeSrvServer
{
public:
  FSpawnObjectServerCallback(const FString &InName, const FString &InType, UObjectController *InObjectController, const bool bInAddObjectInMujoco);

  TSharedPtr<FROSBridgeSrv::SrvRequest> FromJson(TSharedPtr<FJsonObject> JsonObject) const override;

  TSharedPtr<FROSBridgeSrv::SrvResponse> Callback(TSharedPtr<FROSBridgeSrv::SrvRequest> InRequest) override;

private:
  bool bAddObjectInMujoco;

  UObjectController *ObjectController;

  class UStaticMesh *StaticMesh;
};