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
    bool Movable;
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
               bool InMovable,
               const geometry_msgs::Vector3 &InSize,
               const std_msgs::ColorRGBA &InRgba,
               const geometry_msgs::Inertia &InInertial,
               const FString &InMesh) : Name(InName), Type(InType), Movable(InMovable), Size(InSize), Rgba(InRgba), Inertial(InInertial), Mesh(InMesh)
    {
      MsgType = "mujoco_msgs/ObjectInfo";
    }

    ~ObjectInfo() override {}

    FString GetName() const { return Name; }

    void SetName(const FString &InName) { Name = InName; }

    EType GetType() const { return Type; }

    void SetType(const EType &InType) { Type = InType; }

    bool GetMovable() const { return Movable; }

    void SetMovable(bool InMovable) { Movable = InMovable; }

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
      Movable = JsonObject->GetBoolField(TEXT("movable"));
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
             TEXT(", movable = ") + (Movable ? TEXT("true") : TEXT("false")) +
             TEXT(", size = ") + Size.ToString() +
             TEXT(", rgba = ") + Rgba.ToString() +
             TEXT(", inertial = ") + Inertial.ToString() +
             TEXT(", mesh = ") + Mesh + TEXT(" } ");
    }

    virtual TSharedPtr<FJsonObject> ToJsonObject() const override
    {
      TSharedPtr<FJsonObject> ObjectInfoJsonObject = MakeShareable<FJsonObject>(new FJsonObject());
      ObjectInfoJsonObject->SetStringField(TEXT("name"), Name);
      ObjectInfoJsonObject->SetNumberField(TEXT("type"), Type);
      ObjectInfoJsonObject->SetBoolField(TEXT("movable"), Movable);
      ObjectInfoJsonObject->SetObjectField(TEXT("size"), Size.ToJsonObject());
      ObjectInfoJsonObject->SetObjectField(TEXT("rgba"), Rgba.ToJsonObject());
      ObjectInfoJsonObject->SetObjectField(TEXT("inertial"), Inertial.ToJsonObject());
      ObjectInfoJsonObject->SetStringField(TEXT("mesh"), Mesh);
      return ObjectInfoJsonObject;
    }
  };
} // namespace mujoco_msgs