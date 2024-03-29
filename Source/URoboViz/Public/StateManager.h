// Copyright (c) 2023, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "CoreMinimal.h"
#include <string>

// clang-format off
#include "StateManager.generated.h"
// clang-format on

class UObjectController;

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
	TSet<EAttribute> Attributes;
};

UCLASS(Blueprintable, DefaultToInstanced, collapsecategories, hidecategories = Object, editinlinenew)
class UROBOVIZ_API UStateManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Host;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Port;

	UPROPERTY(EditAnywhere)
	TMap<AActor *, FAttributeContainer> SendObjects;

	UPROPERTY(EditAnywhere)
	TMap<AActor *, FAttributeContainer> ReceiveObjects;

public:
	UStateManager();

private:
	TArray<TPair<AActor *, EAttribute>> SendDataArray;

	TArray<TPair<AActor *, EAttribute>> ReceiveDataArray;

	void *context;

	void *socket_client;

	size_t send_buffer_size = 1;

	size_t receive_buffer_size = 1;

	double *send_buffer;

	double *receive_buffer;

	FString SocketAddr;

	std::string socket_addr;

	TMap<AActor *, FAttributeContainer> ReceiveObjectRefs;

	UObjectController *ObjectController;

	FGraphEventRef Task;

private:
	UPROPERTY(VisibleAnywhere)
	bool IsEnable = false;

public:
	void Init();

	void SendMetaData();

	void Deinit();

	void Tick();
};