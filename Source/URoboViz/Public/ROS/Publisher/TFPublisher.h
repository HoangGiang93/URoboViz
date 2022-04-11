// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSPublisher.h"

#include "TFPublisher.generated.h"

class AActor;

UCLASS()
class UROBOVIZ_API UTFPublisher : public UROSPublisher
{
  GENERATED_BODY()

public:
  UTFPublisher();

public:
  void AddObject(AActor *Object) { Objects.Add(Object); }

protected:
  void Publish() override;

public:
  UPROPERTY(EditAnywhere)
  FString FrameId = TEXT("");

private:
  UPROPERTY(EditAnywhere)
  TArray<AActor *> Objects;

  int32 Seq = 0;
};