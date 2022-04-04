// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RobotController.h"
#include "BaseController.generated.h"

/**
 * 
 */
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
