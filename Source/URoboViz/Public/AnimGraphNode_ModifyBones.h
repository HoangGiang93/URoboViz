// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "BonePose.h"
#include "AnimGraphNode_ModifyBones.generated.h"

USTRUCT(BlueprintType)
struct UROBOVIZ_API FAnimNode_ModifyBones : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JointPositions, meta = (PinShownByDefault))
	TMap<FName, float> JointPositions;

public:	
	FAnimNode_ModifyBones();

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

private:
	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	TArray<FBoneReference> BonesToModify;
};

UCLASS(meta=(Keywords = "Modify Transforms"))
class UROBOVIZ_API UAnimGraphNode_ModifyBones : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_ModifyBones Node;
	
public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	// UAnimGraphNode_Base interface
	virtual FString GetNodeCategory() const override;
	virtual void CreateOutputPins() override;
	// End of UAnimGraphNode_Base interface

protected:
	// UAnimGraphNode_Base interface
	virtual FEditorModeID GetEditorMode() const override;
	virtual void CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode) override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface
};
