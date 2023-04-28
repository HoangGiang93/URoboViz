// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSPublisher.h"

#include "ObjectMarkerArrayPublisher.generated.h"

class AActor;

UCLASS()
class UROBOVIZ_API UObjectMarkerArrayPublisher : public UROSPublisher
{
  GENERATED_BODY()

public:
  UObjectMarkerArrayPublisher();

protected:
  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FString FrameId = TEXT("");

  UPROPERTY(EditAnywhere)
  TArray<FString> MeshResourceDirs;

private:
  int32 Seq = 0;
};