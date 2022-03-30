// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_ModifyBones.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationGraphSchema.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

FAnimNode_ModifyBones::FAnimNode_ModifyBones()
{
	
}

void FAnimNode_ModifyBones::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	check(OutBoneTransforms.Num() == 0);

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
	
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
		
		if (BoneToModify.BoneName.ToString().Contains(TEXT("continuous"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
			BoneToModify.BoneName.ToString().Contains(TEXT("revolute"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			FQuat Rotation(FRotator(0.f, JointPositions[BoneToModify.BoneName], 0.f));
			NewBoneTransform.SetRotation(Rotation);
		}
		else if (BoneToModify.BoneName.ToString().Contains(TEXT("prismatic"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			FVector Translation(0.f, 0.f, JointPositions[BoneToModify.BoneName]);
			NewBoneTransform.SetTranslation(Translation);
		}

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTransform, CompactPoseBoneToModifyIndex, EBoneControlSpace::BCS_BoneSpace);

		OutBoneTransforms.Add( FBoneTransform(BoneToModify.GetCompactPoseIndex(BoneContainer), NewBoneTransform) );

		TRACE_ANIM_NODE_VALUE(Output, TEXT("Target"), BoneToModify.BoneName);

		Output.Pose.LocalBlendCSBoneTransforms(OutBoneTransforms, 1.f);
	}
}

bool FAnimNode_ModifyBones::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) 
{
	return !BonesToModify.ContainsByPredicate([&](const FBoneReference& BoneToModify){
				return !BoneToModify.IsValidToEvaluate(RequiredBones);
			});
}

void FAnimNode_ModifyBones::InitializeBoneReferences(const FBoneContainer& RequiredBones) 
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)
	
	for (int32 BoneIndex = 0; BoneIndex < RequiredBones.GetNumBones(); ++BoneIndex)
	{
		FName BoneName = RequiredBones.GetReferenceSkeleton().GetBoneName(BoneIndex);
		if (BoneName.ToString().Contains(TEXT("continuous_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
		BoneName.ToString().Contains(TEXT("prismatic_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
		BoneName.ToString().Contains(TEXT("revolute_bone"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
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


UAnimGraphNode_ModifyBones::UAnimGraphNode_ModifyBones(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FEditorModeID UAnimGraphNode_ModifyBones::GetEditorMode() const
{
	FEditorModeID ModifyBones("AnimGraph.SkeletalControl.ModifyBones");
	return ModifyBones;
}

void UAnimGraphNode_ModifyBones::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
	FAnimNode_ModifyBones* ModifyBones = static_cast<FAnimNode_ModifyBones*>(InPreviewNode);

	// copies Pin values from the internal node to get data which are not compiled yet
	ModifyBones->JointPositions = Node.JointPositions;
}

FText UAnimGraphNode_ModifyBones::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_ModifyBones_Tooltip", "The Transform Bones node alters the transform - i.e. Translation, Rotation, or Scale - of the bones");
}

FText UAnimGraphNode_ModifyBones::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimGraphNode_ModifyBones_Title", "Modify Bones");
}

FString UAnimGraphNode_ModifyBones::GetNodeCategory() const
{
	return TEXT("Modify Bones Node");
}

void UAnimGraphNode_ModifyBones::CreateOutputPins()
{
	CreatePin(EGPD_Output, UAnimationGraphSchema::PC_Struct, FComponentSpacePoseLink::StaticStruct(), TEXT("Pose"));
}