// Fill out your copyright notice in the Description page of Project Settings.

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
  TArray<FString> BoneTails = {TEXT("_continuous_bone"), TEXT("_prismatic_bone"), TEXT("_revolute_bone")};
  for (const FName &BoneName : BoneNames)
  {
    for (const FString &BoneTail : BoneTails)
    {
      if (BoneName.ToString().Contains(BoneTail, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
      {
        FString JointName = BoneName.ToString();
        JointName.RemoveFromEnd(BoneTail);
        DesiredJointPositions.Add(JointName);
        JointNameMap.Add(JointName, BoneName);
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

  for (const TPair<FString, float> &DesiredJointPosition : DesiredJointPositions)
  {
    if (JointNameMap.Contains(DesiredJointPosition.Key))
    {
      FName BoneName = JointNameMap[DesiredJointPosition.Key];
      if (RoboAnim->JointPositions.Contains(BoneName))
      {
        if (BoneName.ToString().Contains(TEXT("_continuous_bone")) || BoneName.ToString().Contains(TEXT("_prismatic_bone")))
        {
          RoboAnim->JointPositions[BoneName] = 0.01 * DesiredJointPosition.Value;
        }
        else if (BoneName.ToString().Contains(TEXT("_revolute_bone")))
        {
          RoboAnim->JointPositions[BoneName] = -FMath::RadiansToDegrees(DesiredJointPosition.Value);
        }
      }
    }
  }
}

void UJointController::SetDesiredJointPosition(FString JointName, float DesiredJointPosition)
{
  if (DesiredJointPositions.Contains(JointName))
  {
    DesiredJointPositions[JointName] = DesiredJointPosition;
  }
}