// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#pragma once

#include "ROSBridgeSrv.h"

#include "mujoco_msgs/ObjectStatus.h"

namespace mujoco_srvs
{
	class SpawnObject : public FROSBridgeSrv
	{

	public:
		SpawnObject()
		{
			SrvType = TEXT("mujoco_srvs/SpawnObject");
		}

		class Request : public SrvRequest
		{
		private:
			TArray<mujoco_msgs::ObjectStatus> Objects;

		public:
			Request() {}

			Request(const TArray<mujoco_msgs::ObjectStatus> &InObjects) : Objects(InObjects) {}

			TArray<mujoco_msgs::ObjectStatus> GetObjects() const { return Objects; }

			void SetObjects(const TArray<mujoco_msgs::ObjectStatus> &InObjects) { Objects = InObjects; }

			virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
			{
				Objects.Empty();
				TArray<TSharedPtr<FJsonValue>> ObjectPtrArray = JsonObject->GetArrayField(TEXT("objects"));
				for (const TSharedPtr<FJsonValue> &ObjectPtr : ObjectPtrArray)
				{
					mujoco_msgs::ObjectStatus Object =
							mujoco_msgs::ObjectStatus::GetFromJson(ObjectPtr->AsObject());
					Objects.Add(Object);
				}
			}

			static Request GetFromJson(TSharedPtr<FJsonObject> JsonObject)
			{
				Request Req;
				Req.FromJson(JsonObject);
				return Req;
			}

			virtual FString ToString() const override
			{
				FString ObjectsString = TEXT("[ ");
				for (const mujoco_msgs::ObjectStatus &Object : Objects)
				{
					ObjectsString += Object.ToString() + TEXT(", ");
				}
				ObjectsString.RemoveFromEnd(TEXT(", "));
				ObjectsString += TEXT(" ]");

				return TEXT("SpawnObject::Request { objects = ") + ObjectsString + TEXT(" } ");
			}

			virtual TSharedPtr<FJsonObject> ToJsonObject() const
			{
				TSharedPtr<FJsonObject> ObjectJsonObject = MakeShareable<FJsonObject>(new FJsonObject());
				TArray<TSharedPtr<FJsonValue>> ObjectPtrArray;
				for (const mujoco_msgs::ObjectStatus &Object : Objects)
				{
					TSharedPtr<FJsonValue> ObjectPtr = MakeShareable(new FJsonValueObject(Object.ToJsonObject()));
					ObjectPtrArray.Add(ObjectPtr);
				}
				ObjectJsonObject->SetArrayField(TEXT("objects"), ObjectPtrArray);
				return ObjectJsonObject;
			}
		};

		class Response : public SrvResponse
		{
		private:
			TArray<FString> Names;

		public:
			Response() {}

			Response(const TArray<FString> &InNames) : Names(InNames) {}

			TArray<FString> GetNames() const { return Names; }

			void SetNames(const TArray<FString> &InNames) { Names = InNames; }

			virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
			{
				TArray<TSharedPtr<FJsonValue>> NamePtrArray = JsonObject->GetArrayField(TEXT("names"));
				Names.Empty();
				for (const TSharedPtr<FJsonValue> &NamePtr : NamePtrArray)
				{
					Names.Add(NamePtr->AsString());
				}
			}

			static Response GetFromJson(TSharedPtr<FJsonObject> JsonObject)
			{
				Response Res;
				Res.FromJson(JsonObject);
				return Res;
			}

			virtual FString ToString() const override
			{
				FString NamesString = TEXT("[ ");
				for (const FString &Name : Names)
				{
					NamesString += Name + TEXT(", ");
				}
				NamesString.RemoveFromEnd(TEXT(", "));
				NamesString += TEXT(" ]");

				return TEXT("SpawnObject::Response { object_states = ") + NamesString + TEXT(" } ");
			}

			virtual TSharedPtr<FJsonObject> ToJsonObject() const
			{
				TSharedPtr<FJsonObject> NameArrayJsonObject = MakeShareable<FJsonObject>(new FJsonObject());
				TArray<TSharedPtr<FJsonValue>> NamesPtrArray;
				for (const FString &Name : Names)
				{
					TSharedPtr<FJsonValue> NamePtr = MakeShareable(new FJsonValueString(Name));
					NamesPtrArray.Add(NamePtr);
				}
				NameArrayJsonObject->SetArrayField(TEXT("names"), NamesPtrArray);
				return NameArrayJsonObject;
			}
		};
	};
} // namespace mujoco_srvs