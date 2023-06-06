// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "GameFramework/Actor.h"
#include "ROSManagerContainer.h"
#include "ZMQManagerContainer.h"
#include "RoboManagerContainer.h"
// clang-format off
#include "RoboManager.generated.h"
// clang-format on

class UObjectController;

UCLASS()
class UROBOVIZ_API ARoboManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoboManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Overridable function called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	URobotController *GetController(const FString &ControllerName) const;

	USensor *GetSensor(const FString &SensorName) const;

	UObjectController *GetObjectController() const { return ObjectController; }

private:
	void Init();

private:
	UPROPERTY(EditAnywhere, Category = "Robot Manager")
	TMap<ASkeletalMeshActor *, FRoboManagerContainer> RobotManager;

	UPROPERTY(EditAnywhere, Category = "ROS Manager")
	FROSManagerContainer ROSManager;

	UPROPERTY(EditAnywhere, Category = "ZMQ Manager")
	FZMQManagerContainer ZMQManager;

	UPROPERTY(VisibleAnywhere, Category = "Object Controller")
	UObjectController *ObjectController;
};
