// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "CoreMinimal.h"
// clang-format off
#include "ZMQManagerContainer.generated.h"
// clang-format on

USTRUCT(Blueprintable)
struct FZMQ
{
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HeaderPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DataPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UZMQPublisher *Publisher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UZMQSubscriber *Subscriber;
};

USTRUCT(Blueprintable)
struct FZMQManagerContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Host;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FZMQManagerContainer> Host;

public:
	FROSManagerContainer()
	{
		Host = TEXT("127.0.0.1");
	}

public:
	void Init();

	void Deinit();

	void Tick();
};