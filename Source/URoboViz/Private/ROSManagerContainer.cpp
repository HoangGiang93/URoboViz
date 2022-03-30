// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSManagerContainer.h"
#include "ROS/Subscriber/ROSSubscriber.h"

void FROSManagerContainer::Init()
{
	Handler = MakeShareable<FROSBridgeHandler>(new FROSBridgeHandler(Host, Port));

	Handler->Connect();

	for (UROSSubscriber *ROSSubscriber : ROSSubscribers)
	{
		if (ROSSubscriber == nullptr)
		{
			continue;
		}
		ROSSubscriber->Connect(Handler);
		ROSSubscriber->Init();
	}
}

void FROSManagerContainer::Deinit()
{
	if (Handler.IsValid())
	{
		Handler->Disconnect();
	}
}

void FROSManagerContainer::Tick()
{
	if (Handler.IsValid())
	{
		Handler->Process();
	}
}