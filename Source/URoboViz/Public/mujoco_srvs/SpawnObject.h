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
			bool Success;

		public:
			Response() {}

			Response(bool InSuccess) : Success(InSuccess) {}

			uint8 GetSuccess() const { return Success; }

			void SetSuccess(uint8 InSuccess) { Success = InSuccess; }

			virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
			{
				Success = JsonObject->GetBoolField("success");
			}

			static Response GetFromJson(TSharedPtr<FJsonObject> JsonObject)
			{
				Response Res;
				Res.FromJson(JsonObject);
				return Res;
			}

			virtual FString ToString() const override
			{
				return TEXT("SpawnObject::Response { success = ") + FString::FromInt(Success) + TEXT(" }");
			}

			virtual TSharedPtr<FJsonObject> ToJsonObject() const
			{
				TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
				Object->SetBoolField("success", Success);
				return Object;
			}
		};
	};
} // namespace mujoco_srvs