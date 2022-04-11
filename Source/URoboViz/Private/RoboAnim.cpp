// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "RoboAnim.h"

URoboAnim::URoboAnim()
{
}

void URoboAnim::NativeBeginPlay()
{
	TArray<FName> BoneNames;
	GetSkelMeshComponent()->GetBoneNames(BoneNames);
	for (const FName &BoneName : BoneNames)
	{
		if (BoneName.ToString().Contains(TEXT("continuous_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				BoneName.ToString().Contains(TEXT("prismatic_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				BoneName.ToString().Contains(TEXT("revolute_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			JointPositions.Add(BoneName);
		}
	}
}

void URoboAnim::NativeUpdateAnimation(float DeltaSeconds)
{
}