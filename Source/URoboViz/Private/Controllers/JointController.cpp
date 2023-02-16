// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "Controllers/JointController.h"
#include "Animation/SkeletalMeshActor.h"
#include "RoboAnim.h"

DEFINE_LOG_CATEGORY_STATIC(LogJointController, Log, All);

UJointController::UJointController()
{
}

void UJointController::Init()
{
  if (GetOwner() == nullptr || GetOwner()->GetSkeletalMeshComponent() == nullptr)
  {
    UE_LOG(LogJointController, Error, TEXT("Owner of %s is nullptr"), *GetName())
    return;
  }

  TArray<FName> BoneNames;
  GetOwner()->GetSkeletalMeshComponent()->GetBoneNames(BoneNames);
  TMap<FString, EJointType> BoneTails = {{TEXT("_continuous_bone"), EJointType::Continuous}, {TEXT("_prismatic_bone"), EJointType::Prismatic}, {TEXT("_revolute_bone"), EJointType::Revolute}};
  for (const FName &BoneName : BoneNames)
  {
    for (const TPair<FString, EJointType> &BoneTail : BoneTails)
    {
      if (BoneName.ToString().Contains(BoneTail.Key, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
      {
        FString JointName = BoneName.ToString();
        JointName.RemoveFromEnd(BoneTail.Key);
        DesiredJointPositions.Add(JointName, FJoint(BoneName, BoneTail.Value));
      }
    }
  }
}

void UJointController::Tick()
{
  if (GetOwner() == nullptr || GetOwner()->GetSkeletalMeshComponent() == nullptr)
  {
    UE_LOG(LogJointController, Error, TEXT("Owner of %s is nullptr"), *GetName())
    return;
  }

  URoboAnim *RoboAnim = Cast<URoboAnim>(GetOwner()->GetSkeletalMeshComponent()->GetAnimInstance());
  if (RoboAnim == nullptr)
  {
    UE_LOG(LogJointController, Error, TEXT("Owner of %s does not have URoboAnim"), *GetName())
    return;
  }

  for (const TPair<FString, FJoint> &DesiredJointPosition : DesiredJointPositions)
  {
    if (RoboAnim->JointPositions.Contains(DesiredJointPosition.Value.BoneName))
    {
      RoboAnim->JointPositions[DesiredJointPosition.Value.BoneName] = DesiredJointPosition.Value.JointPosition;
    }
  }
}

void UJointController::SetDesiredJointPositionFromROS(FString JointName, const float DesiredJointPosition)
{
  JointName = JointName.Replace(TEXT(" "), TEXT("-"));
  if (DesiredJointPositions.Contains(JointName))
  {
    DesiredJointPositions[JointName].SetDesiredJointPositionFromROS(DesiredJointPosition);
  }
}