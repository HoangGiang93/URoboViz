// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ROS/Publisher/LidarPublisher.h"
#include "Sensors/Lidar.h"
#include "sensor_msgs/LaserScan.h"

DEFINE_LOG_CATEGORY_STATIC(LogLidarPublisher, Log, All)

ULidarPublisher::ULidarPublisher()
{
  CommonPublisherParameters.Topic = TEXT("/laser_scan");
  CommonPublisherParameters.MessageType = TEXT("sensor_msgs/LaserScan");
}

void ULidarPublisher::Init()
{
  Super::Init();
  if (GetRoboManager() == nullptr)
  {
    UE_LOG(LogLidarPublisher, Error, TEXT("RoboManager of %s not found"), *GetName())
    return;
  }
  
  Lidar = Cast<ULidar>(GetRoboManager()->GetSensor(LidarName));
}

void ULidarPublisher::Publish()
{
  if (Lidar != nullptr)
  {
    TSharedPtr<sensor_msgs::LaserScan> LaserScanMsg =
        MakeShareable(new sensor_msgs::LaserScan());
    
    FROSTime ROSTime = FROSTime();

    /************************************
     *  BEGIN FIX BUG TF_REPEATED_DATA  *
     ************************************/
    static int32 UniqueInt = 0;
    UniqueInt++;
    if (UniqueInt == 1000)
    {
      UniqueInt = 0;
    }
    ROSTime.NSecs += UniqueInt;
    /************************************
     *  END FIX BUG TF_REPEATED_DATA  *
     ************************************/

    LaserScanMsg->SetHeader(std_msgs::Header(Seq++, ROSTime, FrameId));
    LaserScanMsg->SetAngleMin(Lidar->AngleMin);
    LaserScanMsg->SetAngleMax(Lidar->AngleMax);
    LaserScanMsg->SetAngleIncrement(Lidar->AngleIncrement);
    LaserScanMsg->SetRangeMin(Lidar->RangeMin);
    LaserScanMsg->SetRangeMax(Lidar->RangeMax);
    LaserScanMsg->SetTimeIncrement(Lidar->TimeIncrement);
    LaserScanMsg->SetScanTime(Lidar->GetScanTime());
    LaserScanMsg->SetRanges(Lidar->GetRanges());

    Handler->PublishMsg(CommonPublisherParameters.Topic, LaserScanMsg);
    Handler->Process();
  }
}
