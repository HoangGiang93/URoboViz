// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "RobotController.h"
// clang-format off
#include "BaseController.generated.h"
// clang-format on

UCLASS()
class UROBOVIZ_API UBaseController : public URobotController
{
	GENERATED_BODY()

public:
	UBaseController();

public:
	virtual void Tick() override;

public:
	void SetDesiredBasePoseFromROS(const FVector &InBaseLocation, const FQuat &InBaseRotation);

private:
	UPROPERTY(EditAnywhere)
	FVector BaseLocation;

	UPROPERTY(EditAnywhere)
	FQuat BaseRotation;
};
