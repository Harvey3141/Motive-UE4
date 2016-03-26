// Some copyright should be here...

using UnrealBuildTool;
using System.IO;


public class MotivePlugin : ModuleRules
{

    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public MotivePlugin(TargetInfo Target)
	{

        PublicIncludePaths.AddRange(
            new string[] {
				"MotivePlugin/Public"
				
				// ... add public include paths required here ...
			}
            );
        

        PrivateIncludePaths.AddRange(
            new string[] {
				"MotivePlugin/Private",
				// ... add other private include paths required here ...
			}
            );

		//AddThirdPartyPrivateStaticDependencies(Target, "Spout");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
                "Engine",
                "RHI",
                "RenderCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);

        PrivateDependencyModuleNames.AddRange(
            new string[]
			{
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

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "NatNet/lib", PlatformString, "NatNetLib.lib"));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ThirdPartyPath, "NatNet/lib", PlatformString, "NatNetLib.dll")));

            PublicIncludePaths.AddRange(
                new string[] {
					    Path.Combine(ThirdPartyPath, "NatNet/include")
				    });
        }
	}
}
