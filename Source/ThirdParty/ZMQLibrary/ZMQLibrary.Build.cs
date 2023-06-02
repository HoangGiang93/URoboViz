// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class ZMQLibrary : ModuleRules
{
	public ZMQLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		// Add any include paths for the plugin
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory));

		if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			string ZMQSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "ZMQLibrary", "libzmq.so");
			PublicAdditionalLibraries.Add(ZMQSoPath);
			PublicDelayLoadDLLs.Add(ZMQSoPath);
			RuntimeDependencies.Add(ZMQSoPath);
		}
	}
}
