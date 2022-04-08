// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ROSServiceClient.h"
// clang-format off
#include "SpawnObjectClient.generated.h"
// clang-format on

class AStaticMeshActor;

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

public:
  void CallService(const TArray<AStaticMeshActor *> &Objects);

private:
  UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
  UPROPERTY(EditAnywhere, Category = "Spawn Object Client Message")
  FString FrameId = TEXT("");

  UPROPERTY(EditAnywhere, Category = "Robots To Attach Trigger Volume")
  TArray<class ASkeletalMeshActor *> Robots;

  UPROPERTY(EditAnywhere, Category = "Trigger Volume")
  FVector TriggerVolumeSize = FVector(150.f, 150.f, 150.f);

private:
  int32 Seq = 0;
};

class FSpawnObjectClientCallback : public FROSBridgeSrvClient
{
public:
  FSpawnObjectClientCallback(const FString &InServiceName, const FString &InServiceType);

  void Callback(TSharedPtr<FROSBridgeSrv::SrvResponse> InResponse) override;
};