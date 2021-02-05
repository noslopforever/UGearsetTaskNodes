// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UGearsetTaskNodes : ModuleRules
{
	public UGearsetTaskNodes(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "UGearsetTaskNodes/Private",
            }
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "InputCore",
                "Engine",
				"Slate",
            }
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
            {
            }
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
    }
}
