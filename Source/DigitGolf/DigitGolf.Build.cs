// Some copyright should be here...

using UnrealBuildTool;

public class DigitGolf : ModuleRules
{
	public DigitGolf(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"DigitGolf/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"DigitGolf/Private",
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
                "Json",
                "DesktopPlatform",
                "CoreUObject",
				"InputCore",
				"UnrealEd",
                "Engine",
				"LevelEditor",
				"Slate", "SlateCore"
				
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
