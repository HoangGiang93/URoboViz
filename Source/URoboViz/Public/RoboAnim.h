// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "RoboAnim.generated.h"

/**
 * 
 */
UCLASS()
class UROBOVIZ_API URoboAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	URoboAnim();

public:
	virtual void NativeBeginPlay() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> JointPositions;
};