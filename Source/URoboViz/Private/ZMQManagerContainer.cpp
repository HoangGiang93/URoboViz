// Copyright (c) 2023, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ZMQManagerContainer.h"

#include <chrono>

#include "Engine/StaticMeshActor.h"
#include "Conversions.h"
#include "ZMQLibrary/zmq.hpp"
#include "Json.h"

DEFINE_LOG_CATEGORY_STATIC(LogZMQManagerContainer, Log, All);

bool FZMQManagerContainer::Init()
{
    const TMap<EAttribute, TArray<double>> AttributeMap =
        {
            {EAttribute::Position, {0.0, 0.0, 0.0}},
            {EAttribute::Quaternion, {1.0, 0.0, 0.0, 0.0}},
            {EAttribute::Joint1D, {0.0}}};

    if (SendObjects.Num() > 0 || ReceiveObjects.Num() > 0)
    {
        UE_LOG(LogZMQManagerContainer, Log, TEXT("Initializing the socket connection..."))

        context = zmq_ctx_new();

        socket_client = zmq_socket(context, ZMQ_REQ);
        SocketClientAddr = Host + ":" + FString::FromInt(Port);
        zmq_connect(socket_client, StringCast<ANSICHAR>(*SocketClientAddr).Get());

        TSharedPtr<FJsonObject> HeaderJson = MakeShareable(new FJsonObject);
        HeaderJson->SetStringField("time", "microseconds");
        HeaderJson->SetStringField("simulator", "unreal");

        TSharedPtr<FJsonObject> HeaderSendJson = MakeShareable(new FJsonObject);
        HeaderJson->SetObjectField("send", HeaderSendJson);

        for (const TPair<AActor *, FAttributeContainer> &SendObject : SendObjects)
        {
            if (SendObject.Key == nullptr)
            {
                UE_LOG(LogZMQManagerContainer, Warning, TEXT("Ignore None Object in SendObjects"))
                continue;
            }

            TArray<TSharedPtr<FJsonValue>> AttributeJsonArray;
            for (const EAttribute Attribute : SendObject.Value.Attributes)
            {
                switch (Attribute)
                {
                case EAttribute::Position:
                    AttributeJsonArray.Add(MakeShareable(new FJsonValueString(TEXT("position"))));
                    break;

                case EAttribute::Quaternion:
                    AttributeJsonArray.Add(MakeShareable(new FJsonValueString(TEXT("quaternion"))));
                    break;

                case EAttribute::Joint1D:
                    AttributeJsonArray.Add(MakeShareable(new FJsonValueString(TEXT("joint_1D"))));
                    break;

                default:
                    break;
                }

                SendDataArray.Add(TPair<AActor *, EAttribute>(SendObject.Key, Attribute));
                send_buffer_size += AttributeMap[Attribute].Num();
            }
            HeaderSendJson->SetArrayField(SendObject.Key->GetActorLabel(), AttributeJsonArray);
        }

        TSharedPtr<FJsonObject> HeaderReceiveJson = MakeShareable(new FJsonObject);
        HeaderJson->SetObjectField("receive", HeaderReceiveJson);

        for (const TPair<AActor *, FAttributeContainer> &ReceiveObject : ReceiveObjects)
        {
            if (ReceiveObject.Key == nullptr)
            {
                UE_LOG(LogZMQManagerContainer, Warning, TEXT("Ignore None Object in ReceiveObjects"))
                continue;
            }

            AStaticMeshActor *StaticMeshActor = Cast<AStaticMeshActor>(ReceiveObject.Key);
            if (StaticMeshActor == nullptr)
            {
                UE_LOG(LogZMQManagerContainer, Warning, TEXT("Ignore Non-StaticMeshActor %s in ReceiveObjects"), *ReceiveObject.Key->GetName())
                continue;
            }
            else if (StaticMeshActor->GetStaticMeshComponent() == nullptr)
            {
                UE_LOG(LogZMQManagerContainer, Warning, TEXT("StaticMeshActor %s in ReceiveObjects has None StaticMeshComponent"), *ReceiveObject.Key->GetName())
                continue;
            }

            StaticMeshActor->GetStaticMeshComponent()->SetSimulatePhysics(false);

            TArray<TSharedPtr<FJsonValue>> AttributeJsonArray;
            for (const EAttribute &Attribute : ReceiveObject.Value.Attributes)
            {
                switch (Attribute)
                {
                case EAttribute::Position:
                    AttributeJsonArray.Add(MakeShareable(new FJsonValueString(TEXT("position"))));
                    break;

                case EAttribute::Quaternion:
                    AttributeJsonArray.Add(MakeShareable(new FJsonValueString(TEXT("quaternion"))));
                    break;

                case EAttribute::Joint1D:
                    AttributeJsonArray.Add(MakeShareable(new FJsonValueString(TEXT("joint_1D"))));
                    break;

                default:
                    break;
                }

                ReceiveDataArray.Add(TPair<AActor *, EAttribute>(ReceiveObject.Key, Attribute));
                receive_buffer_size += AttributeMap[Attribute].Num();
            }
            HeaderReceiveJson->SetArrayField(ReceiveObject.Key->GetActorLabel(), AttributeJsonArray);
        }

        FString HeaderString;
        TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<TCHAR>::Create(&HeaderString);
        FJsonSerializer::Serialize(HeaderJson.ToSharedRef(), Writer, true);

        UE_LOG(LogZMQManagerContainer, Log, TEXT("%s"), *HeaderString)

        // Send JSON string over ZMQ
        std::string header_string;
        for (size_t i = 0; i < (HeaderString.Len() + 127) / 128; i++) // Split string into multiple substrings with a length of 128 characters or less
        {
            const int32 StartIndex = i * 128;
            const int32 SubstringLength = FMath::Min(128, HeaderString.Len() - StartIndex);
            const FString Substring = HeaderString.Mid(StartIndex, SubstringLength);
            header_string += StringCast<ANSICHAR>(*Substring).Get();
        }
        zmq_send(socket_client, header_string.c_str(), header_string.size(), 0);

        // Receive buffer sizes over ZMQ
        size_t buffer[2];
        zmq_recv(socket_client, buffer, sizeof(buffer), 0);

        if (buffer[0] != send_buffer_size || buffer[1] != receive_buffer_size)
        {
            UE_LOG(LogZMQManagerContainer, Error, TEXT("Failed to initialize the socket header at %s: send_buffer_size(server = %ld != client = %ld), receive_buffer_size(server = %ld != client = %ld)."), *SocketClientAddr, buffer[0], send_buffer_size, buffer[1], receive_buffer_size)
            return false;
        }
        else
        {
            send_buffer = (double *)calloc(send_buffer_size, sizeof(double));
            receive_buffer = (double *)calloc(receive_buffer_size, sizeof(double));

            UE_LOG(LogZMQManagerContainer, Log, TEXT("Initialized the socket header at %s successfully."), *SocketClientAddr);
            return true;
        }
    }

    return false;
}

void FZMQManagerContainer::Deinit()
{
    UE_LOG(LogZMQManagerContainer, Log, TEXT("Deinitializing the socket connection..."))

    free(send_buffer);
    free(receive_buffer);

    zmq_disconnect(socket_client, StringCast<ANSICHAR>(*SocketClientAddr).Get());
}

void FZMQManagerContainer::Tick()
{
    *send_buffer = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    double *send_buffer_addr = send_buffer + 1;
    for (TPair<AActor *, EAttribute> &SendData : SendDataArray)
    {
        switch (SendData.Value)
        {
        case EAttribute::Position:
        {
            const FVector LocationInROS = FConversions::UToROS(SendData.Key->GetActorLocation());
            *send_buffer_addr++ = LocationInROS.X;
            *send_buffer_addr++ = LocationInROS.Y;
            *send_buffer_addr++ = LocationInROS.Z;
            break;
        }

        case EAttribute::Quaternion:
        {
            const FQuat QuatInROS = FConversions::UToROS(SendData.Key->GetActorQuat());
            *send_buffer_addr++ = QuatInROS.W;
            *send_buffer_addr++ = QuatInROS.X;
            *send_buffer_addr++ = QuatInROS.Y;
            *send_buffer_addr++ = QuatInROS.Z;
            break;
        }

        default:
            break;
        }
    }

    zmq_send(socket_client, send_buffer, send_buffer_size * sizeof(double), 0);

    zmq_recv(socket_client, receive_buffer, receive_buffer_size * sizeof(double), 0);

    double *receive_buffer_addr = receive_buffer + 1;
    for (TPair<AActor *, EAttribute> &ReceiveData : ReceiveDataArray)
    {
        switch (ReceiveData.Value)
        {
        case EAttribute::Position:
        {
            const float X = *receive_buffer_addr++;
            const float Y = *receive_buffer_addr++;
            const float Z = *receive_buffer_addr++;
            const FVector LocationInUnreal = FConversions::ROSToU(FVector(X, Y, Z));
            ReceiveData.Key->SetActorLocation(LocationInUnreal);
            break;
        }

        case EAttribute::Quaternion:
        {
            const float W = *receive_buffer_addr++;
            const float X = *receive_buffer_addr++;
            const float Y = *receive_buffer_addr++;
            const float Z = *receive_buffer_addr++;
            const FQuat QuatInUnreal = FConversions::ROSToU(FQuat(X, Y, Z, W));
            ReceiveData.Key->SetActorRotation(QuatInUnreal);
            break;
        }

        default:
            break;
        }
    }
}