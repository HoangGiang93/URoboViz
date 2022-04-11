// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "Animation/AnimInstance.h"
// clang-format off
#include "RoboAnim.generated.h"
// clang-format on

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