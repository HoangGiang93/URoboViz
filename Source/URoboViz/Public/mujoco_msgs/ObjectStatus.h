// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSBridgeMsg.h"

#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
#include "mujoco_msgs/ObjectInfo.h"
#include "std_msgs/Header.h"

namespace mujoco_msgs
{
  class ObjectStatus : public FROSBridgeMsg
  {
  private:
    std_msgs::Header Header;
    mujoco_msgs::ObjectInfo Info;
    geometry_msgs::Pose Pose;
    geometry_msgs::Twist Velocity;

  public:
    ObjectStatus()
    {
      MsgType = "mujoco_msgs/ObjectStatus";
    }

    ObjectStatus(const std_msgs::Header &InHeader,
                 const mujoco_msgs::ObjectInfo &InInfo,
                 const geometry_msgs::Pose &InPose,
                 const geometry_msgs::Twist &InVelocity) : Header(InHeader), Info(InInfo), Pose(InPose), Velocity(InVelocity)
    {
      MsgType = "mujoco_msgs/ObjectStatus";
    }

    ~ObjectStatus() override {}

    std_msgs::Header GetHeader() const { return Header; }

    void SetHeader(const std_msgs::Header &InHeader) { Header = InHeader; }

    mujoco_msgs::ObjectInfo GetInfo() const { return Info; }

    void SetInfo(const mujoco_msgs::ObjectInfo &InInfo) { Info = InInfo; }

    geometry_msgs::Pose GetPose() const { return Pose; }

    void SetPose(const geometry_msgs::Pose &InPose) { Pose = InPose; }

    geometry_msgs::Twist GetVelocity() const { return Velocity; }

    void SetVelocity(const geometry_msgs::Twist &InVelocity) { Velocity = InVelocity; }

    virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
    {
      Header = std_msgs::Header::GetFromJson(JsonObject->GetObjectField(TEXT("header")));
      Info = mujoco_msgs::ObjectInfo::GetFromJson(JsonObject->GetObjectField(TEXT("info")));
      Pose = geometry_msgs::Pose::GetFromJson(JsonObject->GetObjectField(TEXT("pose")));
      Velocity = geometry_msgs::Twist::GetFromJson(JsonObject->GetObjectField(TEXT("velocity")));
    }

    static ObjectStatus GetFromJson(TSharedPtr<FJsonObject> JsonObject)
    {
      ObjectStatus Result;
      Result.FromJson(JsonObject);
      return Result;
    }

    virtual FString ToString() const override
    {
      return TEXT("ObjectStatus { header = ") + Header.ToString() +
             TEXT(", info = ") + Info.ToString() +
             TEXT(", pose = ") + Pose.ToString() +
             TEXT(", velocity = ") + Velocity.ToString() + TEXT(" } ");
    }

    virtual TSharedPtr<FJsonObject> ToJsonObject() const override
    {
      TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
      Object->SetObjectField(TEXT("header"), Header.ToJsonObject());
      Object->SetObjectField(TEXT("info"), Info.ToJsonObject());
      Object->SetObjectField(TEXT("pose"), Pose.ToJsonObject());
      Object->SetObjectField(TEXT("velocity"), Velocity.ToJsonObject());
      return Object;
    }

    virtual FString ToYamlString() const override
    {
      FString OutputString;
      TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
      FJsonSerializer::Serialize(ToJsonObject().ToSharedRef(), Writer);
      return OutputString;
    }
  };
} // namespace mujoco_msgs