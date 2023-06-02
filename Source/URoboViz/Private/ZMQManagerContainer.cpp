// Copyright (c) 2023, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ZMQManagerContainer.h"

#include "Json.h"
#include "ZMQLibrary/zmq.hpp"

zmq::socket_t socket_header;
zmq::socket_t socket_data;

FString SocketHeaderAddr;
std::string socket_data_addr;

size_t send_data_size = 1;
size_t receive_data_size = 1;

DEFINE_LOG_CATEGORY_STATIC(LogZMQManagerContainer, Log, All);

void FZMQManagerContainer::Init()
{
    const TMap<EAttribute, TArray<double>> AttributeSizeMap =
        {
            {EAttribute::Position, {0.0, 0.0, 0.0}},
            {EAttribute::Quaternion, {1.0, 0.0, 0.0, 0.0}},
            {EAttribute::Joint1D, {0.0}}};

    const TMap<EAttribute, FString> AttributeStringMap =
        {
            {EAttribute::Position, TEXT("position")},
            {EAttribute::Quaternion, TEXT("quaternion")},
            {EAttribute::Joint1D, TEXT("joint_1D")}};

    if (SendObjects.Num() > 0 || ReceiveObjects.Num() > 0)
    {
        UE_LOG(LogZMQManagerContainer, Warning, TEXT("Initializing the socket connection..."))

        zmq::context_t context = zmq::context_t{1};

        socket_header = zmq::socket_t{context, zmq::socket_type::req};
        SocketHeaderAddr = TEXT("tcp://127.0.0.1:7500");
        socket_header.connect("tcp://127.0.0.1:7500");

        // socket_data = zmq::socket_t{context, zmq::socket_type::req};
        // socket_data_addr = std::string(TCHAR_TO_UTF8(*Host)) + ":" + std::to_string(DataPort);
        // socket_data.connect(socket_data_addr);

        TSharedPtr<FJsonObject> HeaderJson = MakeShareable(new FJsonObject);
        HeaderJson->SetStringField("time", "microseconds");
        HeaderJson->SetStringField("simulator", "unreal");

        TSharedPtr<FJsonObject> HeaderSendJson = MakeShareable(new FJsonObject);
        HeaderJson->SetObjectField("send", HeaderSendJson);

        for (const TPair<AActor *, FAttributeContainer> &SendObject : SendObjects)
        {
            TArray<TSharedPtr<FJsonValue>> AttributeJsonArray;
            for (const EAttribute &Attribute : SendObject.Value.Attributes)
            {
                AttributeJsonArray.Add(MakeShareable(new FJsonValueString(AttributeStringMap[Attribute])));
                send_data_size += AttributeSizeMap[Attribute].Num();
            }
            HeaderSendJson->SetArrayField(SendObject.Key->GetActorLabel(), AttributeJsonArray);
        }

        FString HeaderString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&HeaderString);
        FJsonSerializer::Serialize(HeaderJson.ToSharedRef(), Writer);
        std::string header_str = TCHAR_TO_UTF8(*HeaderString);

        // Send JSON string over ZMQ
        zmq::message_t request_header(header_str.size());
        memcpy(request_header.data(), header_str.c_str(), header_str.size());
        socket_header.send(request_header, zmq::send_flags::none);

        // Receive buffer sizes over ZMQ
        // size_t buffer[2];
        // zmq::message_t reply_header(sizeof(buffer));
        // socket_header.recv(reply_header);
        // memcpy(&buffer, reply_header.data(), sizeof(buffer));

        // if (buffer[0] != send_data_size || buffer[1] != receive_data_size)
        // {
        //     UE_LOG(LogZMQManagerContainer, Error, TEXT("Failed to initialize the socket header at %s: send_data_size(server = %ld != client = %ld), receive_data_size(server = %ld != client = %ld)."), *SocketHeaderAddr, buffer[0], send_data_size, buffer[1], receive_data_size)
        // }
        // else
        // {
        //     UE_LOG(LogZMQManagerContainer, Warning, TEXT("Initialized the socket header at %s successfully."), *SocketHeaderAddr);
        // }
        UE_LOG(LogZMQManagerContainer, Warning, TEXT("Initialized the socket header at %s successfully."), *SocketHeaderAddr);
    }
}

void FZMQManagerContainer::Deinit()
{
    UE_LOG(LogZMQManagerContainer, Warning, TEXT("Deinitializing the socket connection..."))

    socket_header.disconnect(std::string(TCHAR_TO_UTF8(*SocketHeaderAddr)));
    // socket_data.disconnect(socket_data_addr);
}

void FZMQManagerContainer::Tick()
{
}