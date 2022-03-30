// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RobotController.h"
#include "JointController.generated.h"

/**
 * 
 */
UCLASS()
class UROBOVIZ_API UJointController : public URobotController
{
	GENERATED_BODY()
	
public:
	UJointController();

public:
	virtual void Tick() override;

protected:
	virtual void Init() override;

public:
	void SetDesiredJointPosition(FString JointName, float DesiredJointPosition);

private:
	UPROPERTY(EditAnywhere)
	TMap<FString, float> DesiredJointPositions;

	UPROPERTY(VisibleAnywhere)
	TMap<FString, FName> JointNameMap;
};
