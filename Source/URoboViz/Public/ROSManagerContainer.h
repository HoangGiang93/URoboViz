// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "CoreMinimal.h"
// clang-format off
#include "ROSManagerContainer.generated.h"
// clang-format on

class FROSBridgeHandler;
class UROSPublisher;
class UROSSubscriber;
class UROSServiceServer;
class UROSServiceClient;

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
	TArray<UROSPublisher *> ROSPublishers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UROSSubscriber *> ROSSubscribers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UROSServiceServer *> ROSServiceServers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UROSServiceClient *> ROSServiceClients;

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

private:
	TSharedPtr<FROSBridgeHandler> Handler;
};