// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSBridgeMsg.h"

#include "geometry_msgs/Inertia.h"
#include "geometry_msgs/Vector3.h"
#include "std_msgs/ColorRGBA.h"

namespace mujoco_msgs
{
  class ObjectInfo : public FROSBridgeMsg
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
    FString Name;
    EType Type;
    geometry_msgs::Vector3 Size;
    std_msgs::ColorRGBA Rgba;
    geometry_msgs::Inertia Inertial;
    FString Mesh;

  public:
    ObjectInfo()
    {
      MsgType = "mujoco_msgs/ObjectInfo";
    }

    ObjectInfo(const FString &InName,
               const EType &InType,
               const geometry_msgs::Vector3 &InSize,
               const std_msgs::ColorRGBA &InRgba,
               const geometry_msgs::Inertia &InInertial,
               const FString &InMesh) : Name(InName), Type(InType), Size(InSize), Rgba(InRgba), Inertial(InInertial), Mesh(InMesh)
    {
      MsgType = "mujoco_msgs/ObjectInfo";
    }

    ~ObjectInfo() override {}

    FString GetName() const { return Name; }

    void SetName(const FString &InName) { Name = InName; }

    EType GetType() const { return Type; }

    void SetType(const EType &InType) { Type = InType; }

    FVector GetSize() const { return Size.GetVector(); }

    void SetSize(const FVector &InSize) { Size = InSize; }

    std_msgs::ColorRGBA GetColor() const { return Rgba; }

    void SetColor(const std_msgs::ColorRGBA &InRgba) { Rgba = InRgba; }

    geometry_msgs::Inertia GetInertial() const { return Inertial; }

    void SetInertial(const geometry_msgs::Inertia &InInertial) { Inertial = InInertial; }

    FString GetMesh() const { return Mesh; }

    void SetMesh(const FString &InMesh) { Mesh = InMesh; }

    virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
    {
      Name = JsonObject->GetStringField(TEXT("name"));
      Type = (EType)(JsonObject->GetNumberField(TEXT("type")));
      Size = geometry_msgs::Vector3::GetFromJson(JsonObject->GetObjectField(TEXT("size")));
      Rgba = std_msgs::ColorRGBA::GetFromJson(JsonObject->GetObjectField(TEXT("rgba")));
      Inertial = geometry_msgs::Inertia::GetFromJson(JsonObject->GetObjectField(TEXT("inertial")));
      Mesh = JsonObject->GetStringField(TEXT("mesh"));
    }

    static ObjectInfo GetFromJson(TSharedPtr<FJsonObject> JsonObject)
    {
      ObjectInfo Result;
      Result.FromJson(JsonObject);
      return Result;
    }

    virtual FString ToString() const override
    {
      return TEXT("ObjectInfo { name = ") + Name +
             TEXT(", type = ") + FString::FromInt(Type) +
             TEXT(", size = ") + Size.ToString() +
             TEXT(", rgba = ") + Rgba.ToString() +
             TEXT(", inertial = ") + Inertial.ToString() +
             TEXT(", mesh = ") + Mesh + TEXT(" } ");
    }

    virtual TSharedPtr<FJsonObject> ToJsonObject() const override
    {
      TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
      Object->SetStringField(TEXT("name"), Name);
      Object->SetNumberField(TEXT("type"), Type);
      Object->SetObjectField(TEXT("size"), Size.ToJsonObject());
      Object->SetObjectField(TEXT("rgba"), Rgba.ToJsonObject());
      Object->SetObjectField(TEXT("inertial"), Inertial.ToJsonObject());
      Object->SetStringField(TEXT("mesh"), Mesh);
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