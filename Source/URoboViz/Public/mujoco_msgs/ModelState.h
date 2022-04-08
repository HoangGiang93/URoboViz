#pragma once
#include "ROSBridgeMsg.h"

#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Vector3.h"

#include "std_msgs/ColorRGBA.h"
#include "std_msgs/Header.h"

namespace mujoco_msgs
{
  class ModelState : public FROSBridgeMsg
  {
  public:
    enum EType : uint8
    {
      CUBE = 0,
      SPHERE = 1,
      CYLINDER = 2,
      MESH = 3
    };

  private:
    std_msgs::Header Header;
    FString Name;
    geometry_msgs::Pose Pose;
    geometry_msgs::Twist Velocity;
    EType Type;
    geometry_msgs::Vector3 Scale;
    std_msgs::ColorRGBA Color;

  public:
    ModelState()
    {
      MsgType = "mujoco_msgs/ModelState";
    }

    ModelState(std_msgs::Header InHeader,
               FString InName,
               geometry_msgs::Pose InPose,
               geometry_msgs::Twist InVelocity,
               EType InType,
               geometry_msgs::Vector3 InScale,
               std_msgs::ColorRGBA InColor) : Header(InHeader), Name(InName), Pose(InPose), Velocity(InVelocity), Type(InType), Scale(InScale), Color(InColor)
    {
      MsgType = "mujoco_msgs/ModelState";
    }

    ~ModelState() override {}

    std_msgs::Header GetHeader() const { return Header; }

    void SetHeader(const std_msgs::Header &InHeader) { Header = InHeader; }

    FString GetName() const { return Name; }

    void SetName(const FString &InName) { Name = InName; }

    geometry_msgs::Pose GetPose() const { return Pose; }

    void SetPose(const geometry_msgs::Pose &InPose) { Pose = InPose; }

    geometry_msgs::Twist GetVelocity() const { return Velocity; }

    void SetVelocity(const geometry_msgs::Twist &InVelocity) { Velocity = InVelocity; }

    EType GetType() const { return Type; }

    void SetType(const EType &InType) { Type = InType; }

    FVector GetScale() const { return Scale.GetVector(); }

    void SetScale(const FVector &InScale) { Scale = InScale; }

    std_msgs::ColorRGBA GetColor() const { return Color; }

    void SetColor(const std_msgs::ColorRGBA &InColor) { Color = InColor; }

    virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
    {
      Header = std_msgs::Header::GetFromJson(JsonObject->GetObjectField(TEXT("header")));
      Name = JsonObject->GetStringField(TEXT("name"));
      Pose = geometry_msgs::Pose::GetFromJson(JsonObject->GetObjectField(TEXT("pose")));
      Velocity = geometry_msgs::Twist::GetFromJson(JsonObject->GetObjectField(TEXT("velocity")));
      Type = (EType)(JsonObject->GetNumberField(TEXT("type")));
      Scale = geometry_msgs::Vector3::GetFromJson(JsonObject->GetObjectField(TEXT("scale")));
      Color = std_msgs::ColorRGBA::GetFromJson(JsonObject->GetObjectField(TEXT("color")));
    }

    static ModelState GetFromJson(TSharedPtr<FJsonObject> JsonObject)
    {
      ModelState Result;
      Result.FromJson(JsonObject);
      return Result;
    }

    virtual FString ToString() const override
    {
      return TEXT("ModelState { header = ") + Header.ToString() +
             TEXT(", name = ") + Name +
             TEXT(", pose = ") + Pose.ToString() +
             TEXT(", velocity = ") + Velocity.ToString() +
             TEXT(", type = ") + FString::FromInt(Type) +
             TEXT(", scale = ") + Scale.ToString() +
             TEXT(", color = ") + Color.ToString() + TEXT(" } ");
    }

    virtual TSharedPtr<FJsonObject> ToJsonObject() const override
    {
      TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
      Object->SetObjectField(TEXT("header"), Header.ToJsonObject());
      Object->SetStringField(TEXT("name"), Name);
      Object->SetObjectField(TEXT("pose"), Pose.ToJsonObject());
      Object->SetObjectField(TEXT("velocity"), Velocity.ToJsonObject());
      Object->SetNumberField(TEXT("type"), Type);
      Object->SetObjectField(TEXT("scale"), Scale.ToJsonObject());
      Object->SetObjectField(TEXT("color"), Color.ToJsonObject());
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