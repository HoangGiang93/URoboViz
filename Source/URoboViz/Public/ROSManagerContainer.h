// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ROSManagerContainer.generated.h"

class FROSBridgeHandler;
class UROSSubscriber;

USTRUCT(Blueprintable)
struct FROSManagerContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Host;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UROSSubscriber *> ROSSubscribers;

public:
	FROSManagerContainer()
	{
		Host = TEXT("127.0.0.1");
		Port = 9393;
	}

public:
	void Init();

	void Deinit();

	void Tick();
};