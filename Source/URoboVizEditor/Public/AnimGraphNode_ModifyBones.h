// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_ModifyBones.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "BonePose.h"
#include "UObject/ObjectMacros.h"
// clang-format off
#include "AnimGraphNode_ModifyBones.generated.h"
// clang-format on

UCLASS(meta = (Keywords = "Modify Transforms"))
class UROBOVIZEDITOR_API UAnimGraphNode_ModifyBones : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_ModifyBones Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	// UAnimGraphNode_Base interface
	virtual FString GetNodeCategory() const override;
	// End of UAnimGraphNode_Base interface

protected:
	// UAnimGraphNode_Base interface
	virtual FEditorModeID GetEditorMode() const override;
	virtual void CopyNodeDataToPreviewNode(FAnimNode_Base *InPreviewNode) override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase *GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface
};
