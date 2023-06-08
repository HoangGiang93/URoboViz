// Copyright Epic Games, Inc. All Rights Reserved.

#include "URoboViz.h"

#include "Core.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"
#include "ZMQLibrary/zmq.hpp"

#define LOCTEXT_NAMESPACE "FURoboVizModule"

void FURoboVizModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("URoboViz")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/ZMQLibrary/libzmq.so"));

	LibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (LibraryHandle)
	{
		// Call the test function in the third party library
		zmq::context_t ctx;
		zmq::socket_t sock(ctx, zmq::socket_type::push);
		sock.bind("inproc://test");
		sock.send(zmq::str_buffer("Hello, world"), zmq::send_flags::dontwait);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load zmq third party library"));
	}
}

void FURoboVizModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FURoboVizModule, URoboViz)