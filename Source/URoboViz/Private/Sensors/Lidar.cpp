// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "Sensors/Lidar.h"
#include "Animation/SkeletalMeshActor.h"
#include "Conversions.h"
#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogLidar, Log, All);

ULidar::ULidar()
{
}

void ULidar::Init()
{
  if (GetWorld() == nullptr)
  {
    UE_LOG(LogLidar, Error, TEXT("World is nullptr"))
    return;
  }

  if (FMath::Abs(AngleIncrement) < 0.001f)
  {
    UE_LOG(LogLidar, Warning, TEXT("AngleIncrement %f is too small, set to default (1 deg)"), AngleIncrement)
    AngleIncrement = PI / 180.f;
  }

  if (AngleMax < AngleMin)
  {
    UE_LOG(LogLidar, Warning, TEXT("AngleMax < AngleMin, set to default (0 deg to 360 deg)"), AngleIncrement)
    AngleMin = 0.f;
    AngleMax = 2 * PI;
  }

  Ranges.Reserve(FMath::CeilToInt((AngleMax - AngleMin) / AngleIncrement));

  ScanTimeStart = GetWorld()->GetTimeSeconds();

  TraceParams.AddIgnoredActor(GetOwner());
  TraceParams.bTraceComplex = true;
  TraceParams.bReturnPhysicalMaterial = false;

  AngleStart = AngleMin;
  AngleEnd = AngleMax;
}

void ULidar::Tick(float DeltaTime)
{
  if (GetOwner() == nullptr || GetOwner()->GetSkeletalMeshComponent() == nullptr)
  {
    UE_LOG(LogLidar, Error, TEXT("Owner or SkeletalMeshComponent of %s is nullptr"), *GetName())
    return;
  }

  if (GetWorld() == nullptr)
  {
    UE_LOG(LogLidar, Error, TEXT("World is nullptr"))
    return;
  }

  if (GetWorld()->GetTimeSeconds() - ScanTimeStart < TimeIncrement)
  {
    return;
  }

  ScanTimeStart = GetWorld()->GetTimeSeconds();

  const float AngleDistance = AngleMax - AngleMin;

  AngleIncrement = FMath::Clamp(AngleIncrement, 0.001f, MAX_flt);

  Ranges.Empty();
  Ranges.Reserve(FMath::CeilToInt(AngleDistance / AngleIncrement));

  const FVector LidarLocation = GetOwner()->GetSkeletalMeshComponent()->GetBoneLocation(BoneName);
  const FQuat LidarQuat = GetOwner()->GetSkeletalMeshComponent()->GetBoneQuaternion(BoneName);

  if (TimeIncrement > DeltaTime) // time to scan an angle is longer than DeltaTime => scan an angle
  {
    AngleEnd = AngleStart + AngleIncrement;
  }
  else if (TimeIncrement * AngleDistance > DeltaTime) // time to scan fully is longer than DeltaTime => scan partly
  {
    AngleEnd = AngleStart + AngleIncrement * DeltaTime / TimeIncrement;
  }
  else // time to scan fully is shorter than DeltaTime => scan fully
  {
    AngleStart = AngleMin;
    AngleEnd = AngleMax;
  }

  const float ScanTime = GetScanTime();

  for (float Angle = AngleMin; Angle < AngleMax; Angle += AngleIncrement)
  {
    if (Angle >= AngleStart && Angle <= AngleEnd)
    {
      float AngleInDeg = FMath::RadiansToDegrees(Angle);
      float RangeMinInCm = FConversions::MToCm(RangeMin);
      float RangeMaxInCm = FConversions::MToCm(RangeMax);

      FRotator ResultRotation(0.f, -AngleInDeg, 0.f);

      FVector StartTrace = LidarLocation + RangeMinInCm * LidarQuat.RotateVector(ResultRotation.Vector());
      FVector EndTrace = LidarLocation + RangeMaxInCm * LidarQuat.RotateVector(ResultRotation.Vector());

      FHitResult HitInfo(EForceInit::ForceInit);
      bool Hit = GetWorld()->LineTraceSingleByObjectType(
          HitInfo,
          StartTrace,
          EndTrace,
          ObjectParams,
          TraceParams);

      if (Hit)
      {
        EndTrace = LidarLocation + (RangeMinInCm + HitInfo.Distance) * LidarQuat.RotateVector(ResultRotation.Vector());
#if WITH_EDITOR
        if (bDrawLines)
        {
          DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, ScanTime, 0, 1.f);
        }
        if (bDrawPoints)
        {
          DrawDebugPoint(GetWorld(), EndTrace, 10, FColor::Red, false, ScanTime, 0);
        }
#endif
      }
      Ranges.Add(FConversions::CmToM(RangeMinInCm + HitInfo.Distance));
    }
    else
    {
      Ranges.Add(0.f);
    }
  }

  AngleStart = AngleEnd;

  if (!LastTransform.Equals(GetOwner()->GetActorTransform()))
  {
    FlushPersistentDebugLines(GetWorld());
    LastTransform = GetOwner()->GetActorTransform();
  }
  else if ((AngleStart >= AngleMax && TimeIncrement * AngleDistance > DeltaTime))
  {
    FlushPersistentDebugLines(GetWorld());
  }

  while (AngleStart >= AngleMax)
  {
    AngleStart -= AngleDistance;
  }
}
