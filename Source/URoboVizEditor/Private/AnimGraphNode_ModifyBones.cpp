// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "AnimGraphNode_ModifyBones.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_ModifyBones::UAnimGraphNode_ModifyBones(const FObjectInitializer &ObjectInitializer)
		: Super(ObjectInitializer)
{
}

FEditorModeID UAnimGraphNode_ModifyBones::GetEditorMode() const
{
	FEditorModeID ModifyBones("AnimGraph.SkeletalControl.ModifyBones");
	return ModifyBones;
}

void UAnimGraphNode_ModifyBones::CopyNodeDataToPreviewNode(FAnimNode_Base *InPreviewNode)
{
	FAnimNode_ModifyBones *ModifyBones = static_cast<FAnimNode_ModifyBones *>(InPreviewNode);

	ModifyBones->JointPositions = Node.JointPositions;
}

FText UAnimGraphNode_ModifyBones::GetControllerDescription() const
{
	return LOCTEXT("TransformModifyBones", "Transform (Modify) Bones");
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

#undef LOCTEXT_NAMESPACE