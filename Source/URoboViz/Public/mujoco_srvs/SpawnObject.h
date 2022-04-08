#pragma once

#include "ROSBridgeSrv.h"
#include "mujoco_msgs/ModelState.h"

namespace mujoco_srvs
{
	class SpawnObject : public FROSBridgeSrv
	{

	public:
		SpawnObject()
		{
			SrvType = TEXT("mujoco_srvs/ModelState");
		}

		class Request : public SrvRequest
		{
		private:
			TArray<mujoco_msgs::ModelState> ModelStates;

		public:
			Request(const TArray<mujoco_msgs::ModelState> &InModelStates) : ModelStates(InModelStates) {}

			Request() {}

			TArray<mujoco_msgs::ModelState> GetModelStates() const { return ModelStates; }

			void SetModelStates(const TArray<mujoco_msgs::ModelState> &InModelStates) { ModelStates = InModelStates; }

			virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
			{
				TArray<TSharedPtr<FJsonValue>> ModelStatePtrArray = JsonObject->GetArrayField(TEXT("model_states"));
				for (const TSharedPtr<FJsonValue> &ModelStatePtr : ModelStatePtrArray)
				{
					mujoco_msgs::ModelState ModelState =
							mujoco_msgs::ModelState::GetFromJson(ModelStatePtr->AsObject());
					ModelStates.Add(ModelState);
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
				FString ModelStatesString = TEXT("[ ");
				for (const mujoco_msgs::ModelState &ModelState : ModelStates)
				{
					ModelStatesString += ModelState.ToString() + TEXT(", ");
				}
				ModelStatesString.RemoveFromEnd(TEXT(", "));
				ModelStatesString += TEXT(" ]");

				return TEXT("SpawnObject::Request { model_states = ") + ModelStatesString + TEXT(" } ");
			}

			virtual TSharedPtr<FJsonObject> ToJsonObject() const
			{
				TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());
				TArray<TSharedPtr<FJsonValue>> ModelStatePtrArray;
				for (const mujoco_msgs::ModelState &ModelState : ModelStates)
				{
					TSharedPtr<FJsonValue> ModelStatePtr = MakeShareable(new FJsonValueObject(ModelState.ToJsonObject()));
					ModelStatePtrArray.Add(ModelStatePtr);
				}
				Object->SetArrayField(TEXT("model_states"), ModelStatePtrArray);
				return Object;
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
				Response Resp;
				Resp.FromJson(JsonObject);
				return Resp;
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