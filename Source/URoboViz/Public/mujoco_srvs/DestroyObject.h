#pragma once

#include "ROSBridgeSrv.h"

#include "mujoco_msgs/ObjectState.h"

namespace mujoco_srvs
{
	class DestroyObject : public FROSBridgeSrv
	{

	public:
		DestroyObject()
		{
			SrvType = TEXT("mujoco_srvs/DestroyObject");
		}

		class Request : public SrvRequest
		{
		private:
			TArray<FString> Names;

		public:
			Request(const TArray<FString> &InNames) : Names(InNames) {}

			Request() {}
			
			TArray<FString> GetNames() const { return Names; }

			void SetNames(const TArray<FString> &InNames) { Names = InNames; }

			virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
			{
				TArray<TSharedPtr<FJsonValue>> NamePtrArray = JsonObject->GetArrayField(TEXT("names"));
				for (const TSharedPtr<FJsonValue> &NamePtr : NamePtrArray)
				{
					FString Name = NamePtr->AsString();
					Names.Add(Name);
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
				FString NamesString = TEXT("[ ");
				for (const FString &Name : Names)
				{
					NamesString += Name + TEXT(", ");
				}
				NamesString.RemoveFromEnd(TEXT(", "));
				NamesString += TEXT(" ]");

				return TEXT("DestroyObject::Request { object_states = ") + NamesString + TEXT(" } ");
			}

			virtual TSharedPtr<FJsonObject> ToJsonObject() const
			{
				TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
				TArray<TSharedPtr<FJsonValue>> NamesPtrArray;
				for (const FString &Name : Names)
				{
					TSharedPtr<FJsonValue> NamePtr = MakeShareable(new FJsonValueString(Name));
					NamesPtrArray.Add(NamePtr);
				}
				Object->SetArrayField(TEXT("names"), NamesPtrArray);
				return Object;
			}
		};

		class Response : public SrvResponse
		{
		private:
			TArray<mujoco_msgs::ObjectState> ObjectStates;

		public:
			Response() {}

			Response(const TArray<mujoco_msgs::ObjectState> &InObjectStates) : ObjectStates(InObjectStates) {}

			TArray<mujoco_msgs::ObjectState> GetObjectStates() const { return ObjectStates; }

			void SetObjectStates(const TArray<mujoco_msgs::ObjectState> &InObjectStates) { ObjectStates = InObjectStates; }

			virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
			{
				ObjectStates.Empty();
				TArray<TSharedPtr<FJsonValue>> ObjectStatePtrArray = JsonObject->GetArrayField(TEXT("object_states"));
				for (const TSharedPtr<FJsonValue> &ObjectStatePtr : ObjectStatePtrArray)
				{
					mujoco_msgs::ObjectState ObjectState =
							mujoco_msgs::ObjectState::GetFromJson(ObjectStatePtr->AsObject());
					ObjectStates.Add(ObjectState);
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
				FString ObjectStatesString = TEXT("[ ");
				for (const mujoco_msgs::ObjectState &ObjectState : ObjectStates)
				{
					ObjectStatesString += ObjectState.ToString() + TEXT(", ");
				}
				ObjectStatesString.RemoveFromEnd(TEXT(", "));
				ObjectStatesString += TEXT(" ]");

				return TEXT("DestroyObject::Response { object_states = ") + ObjectStatesString + TEXT(" } ");
			}

			virtual TSharedPtr<FJsonObject> ToJsonObject() const
			{
				TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
				TArray<TSharedPtr<FJsonValue>> ObjectStatePtrArray;
				for (const mujoco_msgs::ObjectState &ObjectState : ObjectStates)
				{
					TSharedPtr<FJsonValue> ObjectStatePtr = MakeShareable(new FJsonValueObject(ObjectState.ToJsonObject()));
					ObjectStatePtrArray.Add(ObjectStatePtr);
				}
				Object->SetArrayField(TEXT("object_states"), ObjectStatePtrArray);
				return Object;
			}
		};
	};
} // namespace mujoco_srvs