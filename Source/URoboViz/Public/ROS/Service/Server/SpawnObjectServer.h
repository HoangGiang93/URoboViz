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

protected:
  virtual void CreateServiceServer() override;
};

class FSpawnObjectServerCallback final : public FROSBridgeSrvServer
{
public:
  FSpawnObjectServerCallback(const FString &InName, const FString &InType, UObjectController *InObjectController);

  TSharedPtr<FROSBridgeSrv::SrvRequest> FromJson(TSharedPtr<FJsonObject> JsonObject) const override;

  TSharedPtr<FROSBridgeSrv::SrvResponse> Callback(TSharedPtr<FROSBridgeSrv::SrvRequest> InRequest) override;

private:
  UObjectController *ObjectController;

  class UStaticMesh *StaticMesh;
};