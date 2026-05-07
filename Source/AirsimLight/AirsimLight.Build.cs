using UnrealBuildTool;

public class AirsimLight : ModuleRules
{
    public AirsimLight(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent"  , "LidarPointCloudRuntime" 
            }
        );

        PrivateDependencyModuleNames.AddRange(new string[] { "Kismet"
        });
    }
}
