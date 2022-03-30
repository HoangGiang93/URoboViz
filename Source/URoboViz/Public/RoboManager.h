// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ROSManagerContainer.h"
#include "RoboManagerContainer.h"
#include "RoboManager.generated.h"

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
	URobotController *GetController(FString ControllerName) const;

private:
	void Init();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Manager")
	TMap<ASkeletalMeshActor *, FRoboManagerContainer> RobotManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ROS Manager")
	FROSManagerContainer ROSManager;
};
