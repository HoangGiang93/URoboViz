// Fill out your copyright notice in the Description page of Project Settings.


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