// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSBridgeMsg.h"

#include "mujoco_msgs/ObjectState.h"
#include "std_msgs/Header.h"

namespace mujoco_msgs
{
  class ObjectStateArray : public FROSBridgeMsg
  {
  private:
    std_msgs::Header Header;
    TArray<mujoco_msgs::ObjectState> ObjectStates;

  public:
    ObjectStateArray()
    {
      MsgType = "mujoco_msgs/ObjectStateArray";
    }

    ObjectStateArray(const std_msgs::Header &InHeader,
                     const TArray<mujoco_msgs::ObjectState> &InObjectStates) : Header(InHeader), ObjectStates(InObjectStates)
    {
      MsgType = "mujoco_msgs/ObjectStateArray";
    }

    ~ObjectStateArray() override {}

    std_msgs::Header GetHeader() const { return Header; }

    void SetHeader(const std_msgs::Header &InHeader) { Header = InHeader; }

    TArray<mujoco_msgs::ObjectState> GetObjectStates() const { return ObjectStates; }

    void SetObjectStates(const TArray<mujoco_msgs::ObjectState> &InObjectStates) { ObjectStates = InObjectStates; }

    virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
    {
      Header = std_msgs::Header::GetFromJson(JsonObject->GetObjectField(TEXT("header")));
      ObjectStates.Empty();
      TArray<TSharedPtr<FJsonValue>> ObjectStatePtrArray = JsonObject->GetArrayField(TEXT("object_states"));
      for (const TSharedPtr<FJsonValue> &ObjectStatePtr : ObjectStatePtrArray)
      {
        ObjectStates.Add(mujoco_msgs::ObjectState::GetFromJson(ObjectStatePtr->AsObject()));
      }
    }

    static ObjectStateArray GetFromJson(TSharedPtr<FJsonObject> JsonObject)
    {
      ObjectStateArray Result;
      Result.FromJson(JsonObject);
      return Result;
    }

    virtual FString ToString() const override
    {
      FString ObjectStatesString = TEXT("[ ");
      for (const mujoco_msgs::ObjectState &ObjectState : ObjectStates)
      {
        ObjectStatesString += ObjectState.ToString() + TEXT(", ");
      }
      ObjectStatesString.RemoveFromEnd(TEXT(", "));
      ObjectStatesString += TEXT(" ]");

      return TEXT("ObjectState { header = ") + Header.ToString() +
             TEXT(", object_states = ") + ObjectStatesString + TEXT(" } ");
    }

    virtual TSharedPtr<FJsonObject> ToJsonObject() const override
    {
      TSharedPtr<FJsonObject> ObjectStateArrayJsonObject = MakeShareable<FJsonObject>(new FJsonObject());

      ObjectStateArrayJsonObject->SetObjectField(TEXT("header"), Header.ToJsonObject());

      TArray<TSharedPtr<FJsonValue>> ObjectStatePtrArray;
      for (const mujoco_msgs::ObjectState &ObjectState : ObjectStates)
      {
        TSharedPtr<FJsonValue> ObjectStatePtr = MakeShareable(new FJsonValueObject(ObjectState.ToJsonObject()));
        ObjectStatePtrArray.Add(ObjectStatePtr);
      }
      ObjectStateArrayJsonObject->SetArrayField(TEXT("object_states"), ObjectStatePtrArray);
      return ObjectStateArrayJsonObject;
    }
  };
} // namespace mujoco_msgs