#pragma once

#include "ROS/ROSBase.h"
#include "ROSBridgeSrvServer.h"
// clang-format off
#include "ROSServiceServer.generated.h"
// clang-format on

USTRUCT()
struct FROSServiceServerParameter
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FString ServiceName;

  UPROPERTY(VisibleAnywhere)
  FString ServiceType;
};

UCLASS()
class UROBOVIZ_API UROSServiceServer : public UROSBase
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  FROSServiceServerParameter CommonServiceServerParameters;

protected:
  virtual void Init() override;

protected:
  virtual void CreateServiceServer() {}

protected:
  TSharedPtr<FROSBridgeSrvServer> ServiceServer;
};