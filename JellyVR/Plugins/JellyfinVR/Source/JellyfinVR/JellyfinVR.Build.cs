// Copyright JellyVR Project. All Rights Reserved.

using UnrealBuildTool;

public class JellyfinVR : ModuleRules
{
	public JellyfinVR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// C++20 required for UE 5.7+
		CppStandard = CppStandardVersion.Cpp20;

		// Add module source directory to include paths
		PublicIncludePaths.Add(ModuleDirectory);
		PrivateIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"HTTP",
				"Json",
				"JsonUtilities",
				"MediaAssets",
				"UMG",
				"Slate",
				"SlateCore",
				"HeadMountedDisplay",
				"XRBase"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"RenderCore",
				"RHI",
				"Projects",
				"ImageWrapper",
				"AudioMixer",
				"SignalProcessing"
			}
		);

		// Platform-specific dependencies
		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.Add("OpenXRHMD");
			PrivateDependencyModuleNames.Add("OpenXRHandTracking");
		}

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
