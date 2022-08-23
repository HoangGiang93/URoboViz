// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "AnimNode_ModifyBones.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_ModifyBones::FAnimNode_ModifyBones()
{
}

void FAnimNode_ModifyBones::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext &Output, TArray<FBoneTransform> &OutBoneTransforms)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	check(OutBoneTransforms.Num() == 0);

	const FBoneContainer &BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();
	for (const FBoneReference &BoneToModify : BonesToModify)
	{
		if (!JointPositions.Contains(BoneToModify.BoneName))
		{
			continue;
		}

		FCompactPoseBoneIndex CompactPoseBoneToModifyIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);
		FTransform NewBoneTransform = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModifyIndex);

		// Convert to Bone Space.
		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTransform, CompactPoseBoneToModifyIndex, EBoneControlSpace::BCS_BoneSpace);

		if (BoneToModify.BoneName.ToString().Contains(TEXT("_continuous_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				BoneToModify.BoneName.ToString().Contains(TEXT("_revolute_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			FQuat Rotation(FRotator(JointPositions[BoneToModify.BoneName], 0.f, 0.f));
			NewBoneTransform.SetRotation(Rotation);
		}
		else if (BoneToModify.BoneName.ToString().Contains(TEXT("_prismatic_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			FVector Translation(0.f, JointPositions[BoneToModify.BoneName], 0.f);
			NewBoneTransform.SetTranslation(Translation);
		}

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTransform, CompactPoseBoneToModifyIndex, EBoneControlSpace::BCS_BoneSpace);

		OutBoneTransforms.Add(FBoneTransform(BoneToModify.GetCompactPoseIndex(BoneContainer), NewBoneTransform));

		TRACE_ANIM_NODE_VALUE(Output, TEXT("Target"), BoneToModify.BoneName);

		Output.Pose.LocalBlendCSBoneTransforms(OutBoneTransforms, 1.f);
	}
}

bool FAnimNode_ModifyBones::IsValidToEvaluate(const USkeleton *Skeleton, const FBoneContainer &RequiredBones)
{
	return !BonesToModify.ContainsByPredicate([&](const FBoneReference &BoneToModify)
																						{ return !BoneToModify.IsValidToEvaluate(RequiredBones); });
}

void FAnimNode_ModifyBones::InitializeBoneReferences(const FBoneContainer &RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)

	for (int32 BoneIndex = 0; BoneIndex < RequiredBones.GetNumBones(); ++BoneIndex)
	{
		FName BoneName = RequiredBones.GetReferenceSkeleton().GetBoneName(BoneIndex);
		if (BoneName.ToString().Contains(TEXT("_continuous_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				BoneName.ToString().Contains(TEXT("_prismatic_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				BoneName.ToString().Contains(TEXT("_revolute_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			FBoneReference BoneToModify(BoneName);
			if (!BonesToModify.Contains(BoneToModify))
			{
				BoneToModify.Initialize(RequiredBones);
				BonesToModify.Add(BoneToModify);
			}
		}
	}
}