// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "RobotController.h"
// clang-format off
#include "JointController.generated.h"
// clang-format on

UENUM()
enum class EJointType : uint8
{
	Revolute,
	Continuous,
	Prismatic
};

USTRUCT(Blueprintable)
struct FJoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;

	UPROPERTY(VisibleAnywhere)
	EJointType JointType = EJointType::Revolute;

	UPROPERTY(EditAnywhere)
	float JointPosition = 0.f;

	FJoint()
	{
	}

	FJoint(const FName &InBoneName, const EJointType &InJointType) : BoneName(InBoneName), JointType(InJointType)
	{
	}

	void SetDesiredJointPositionFromROS(float DesiredJointPosition)
	{
		switch (JointType)
		{
		case EJointType::Revolute:
			JointPosition = -FMath::RadiansToDegrees(DesiredJointPosition);
			break;

		case EJointType::Continuous:
			JointPosition = -FMath::RadiansToDegrees(DesiredJointPosition);
			break;

		case EJointType::Prismatic:
			JointPosition = -DesiredJointPosition;
			break;

		default:
			break;
		}
	}
};

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
	void SetDesiredJointPositionFromROS(FString JointName, const float DesiredJointPosition);

private:
	UPROPERTY(EditAnywhere)
	TMap<FString, FJoint> DesiredJointPositions;
};
