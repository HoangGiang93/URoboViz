// Copyright (c) 2023, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "CoreMinimal.h"
// clang-format off
#include "ZMQManagerContainer.generated.h"
// clang-format on

UENUM()
enum class EAttribute : uint8
{
	Position,
	Quaternion,
	Joint1D
};

USTRUCT(Blueprintable)
struct FAttributeContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAttribute> Attributes;
};

USTRUCT(Blueprintable)
struct FZMQManagerContainer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Host;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HeaderPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DataPort;

	UPROPERTY(EditAnywhere)
	TMap<AActor *, FAttributeContainer> SendObjects;

	UPROPERTY(EditAnywhere)
	TMap<AActor *, FAttributeContainer> ReceiveObjects;

public:
	FZMQManagerContainer()
	{
		Host = TEXT("tcp://127.0.0.1");
		HeaderPort = 7500;
		DataPort = 7600;
	}

public:
	void Init();

	void Deinit();

	void Tick();
};