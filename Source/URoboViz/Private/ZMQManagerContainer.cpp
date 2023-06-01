// Copyright (c) 2022, Hoang Giang Nguyen - Institute for Artificial Intelligence, University Bremen

#include "ZMQManagerContainer.h"
#include "ZMQ/Publisher/ZMQPublisher.h"
#include "ZMQ/Subscriber/ZMQSubscriber.h"

void FZMQManagerContainer::Init()
{
	for (UZMQPublisher *ZMQPublisher : ZMQPublishers)
	{
		if (ZMQPublisher == nullptr)
		{
			continue;
		}
		ZMQPublisher->Connect(Host);
	}
	for (UZMQSubscriber *ZMQSubscriber : ZMQSubscribers)
	{
		if (ZMQSubscriber == nullptr)
		{
			continue;
		}
		ZMQSubscriber->Connect(Host);
	}
}

void FZMQManagerContainer::Deinit()
{
	for (UZMQPublisher *ZMQPublisher : ZMQPublishers)
	{
		if (ZMQPublisher == nullptr)
		{
			continue;
		}
		ZMQPublisher->Disconnect();
	}
	for (UZMQSubscriber *ZMQSubscriber : ZMQSubscribers)
	{
		if (ZMQSubscriber == nullptr)
		{
			continue;
		}
		ZMQSubscriber->Disconnect();
	}
}

void FZMQManagerContainer::Tick()
{	
	for (UZMQPublisher *ZMQPublisher : ZMQPublishers)
	{
		if (ZMQPublisher == nullptr)
		{
			continue;
		}
		ZMQPublisher->Tick();
	}
	for (UZMQSubscriber *ZMQSubscriber : ZMQSubscribers)
	{
		if (ZMQSubscriber == nullptr)
		{
			continue;
		}
		ZMQSubscriber->Tick();
	}
}