// Fill out your copyright notice in the Description page of Project Settings.

#include "ROSManagerContainer.h"
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
}