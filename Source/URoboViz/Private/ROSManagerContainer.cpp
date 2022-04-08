// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSManagerContainer.h"
#include "ROS/Service/Client/ROSServiceClient.h"
#include "ROS/Service/Server/ROSServiceServer.h"
#include "ROS/Subscriber/ROSSubscriber.h"

void FROSManagerContainer::Init()
{
	for (UROSSubscriber *ROSSubscriber : ROSSubscribers)
	{
		if (ROSSubscriber == nullptr)
		{
			continue;
		}
		ROSSubscriber->Connect(Host, Port);
	}
	for (UROSServiceServer *ROSServiceServer : ROSServiceServers)
	{
		if (ROSServiceServer == nullptr)
		{
			continue;
		}
		ROSServiceServer->Connect(Host, Port);
	}
	for (UROSServiceClient *ROSServiceClient : ROSServiceClients)
	{
		if (ROSServiceClient == nullptr)
		{
			continue;
		}
		ROSServiceClient->Connect(Host, Port);
	}
}

void FROSManagerContainer::Deinit()
{
	for (UROSSubscriber *ROSSubscriber : ROSSubscribers)
	{
		if (ROSSubscriber == nullptr)
		{
			continue;
		}
		ROSSubscriber->Disconnect();
	}
	for (UROSServiceServer *ROSServiceServer : ROSServiceServers)
	{
		if (ROSServiceServer == nullptr)
		{
			continue;
		}
		ROSServiceServer->Disconnect();
	}
	for (UROSServiceClient *ROSServiceClient : ROSServiceClients)
	{
		if (ROSServiceClient == nullptr)
		{
			continue;
		}
		ROSServiceClient->Disconnect();
	}
}

void FROSManagerContainer::Tick()
{
	for (UROSSubscriber *ROSSubscriber : ROSSubscribers)
	{
		if (ROSSubscriber == nullptr)
		{
			continue;
		}
		ROSSubscriber->Tick();
	}
	for (UROSServiceServer *ROSServiceServer : ROSServiceServers)
	{
		if (ROSServiceServer == nullptr)
		{
			continue;
		}
		ROSServiceServer->Tick();
	}
	for (UROSServiceClient *ROSServiceClient : ROSServiceClients)
	{
		if (ROSServiceClient == nullptr)
		{
			continue;
		}
		ROSServiceClient->Tick();
	}
}