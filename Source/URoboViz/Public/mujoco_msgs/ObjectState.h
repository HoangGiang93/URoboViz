#pragma once

#include "ROSBridgeMsg.h"

#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Vector3.h"
#include "std_msgs/ColorRGBA.h"
#include "std_msgs/Header.h"

namespace mujoco_msgs
{
  class ObjectState : public FROSBridgeMsg
  {
  private:
    std_msgs::Header Header;
    FString Name;
    geometry_msgs::Pose Pose;
    geometry_msgs::Twist Velocity;

  public:
    ObjectState()
    {
      MsgType = "mujoco_msgs/ObjectState";
    }

    ObjectState(const std_msgs::Header &InHeader,
                const FString &InName,
                const geometry_msgs::Pose &InPose,
                const geometry_msgs::Twist &InVelocity) : Header(InHeader), Name(InName), Pose(InPose), Velocity(InVelocity)
    {
      MsgType = "mujoco_msgs/ObjectState";
    }

    ~ObjectState() override {}

    std_msgs::Header GetHeader() const { return Header; }

    void SetHeader(const std_msgs::Header &InHeader) { Header = InHeader; }

    FString GetName() const { return Name; }

    void SetName(const FString &InName) { Name = InName; }

    geometry_msgs::Pose GetPose() const { return Pose; }

    void SetPose(const geometry_msgs::Pose &InPose) { Pose = InPose; }

    geometry_msgs::Twist GetVelocity() const { return Velocity; }

    void SetVelocity(const geometry_msgs::Twist &InVelocity) { Velocity = InVelocity; }

    virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
    {
      Header = std_msgs::Header::GetFromJson(JsonObject->GetObjectField(TEXT("header")));
      Name = JsonObject->GetStringField(TEXT("name"));
      Pose = geometry_msgs::Pose::GetFromJson(JsonObject->GetObjectField(TEXT("pose")));
      Velocity = geometry_msgs::Twist::GetFromJson(JsonObject->GetObjectField(TEXT("velocity")));
    }

    static ObjectState GetFromJson(TSharedPtr<FJsonObject> JsonObject)
    {
      ObjectState Result;
      Result.FromJson(JsonObject);
      return Result;
    }

    virtual FString ToString() const override
    {
      return TEXT("ObjectState { header = ") + Header.ToString() +
             TEXT(", name = ") + Name +
             TEXT(", pose = ") + Pose.ToString() +
             TEXT(", velocity = ") + Velocity.ToString() + TEXT(" } ");
    }

    virtual TSharedPtr<FJsonObject> ToJsonObject() const override
    {
      TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
      Object->SetObjectField(TEXT("header"), Header.ToJsonObject());
      Object->SetStringField(TEXT("name"), Name);
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