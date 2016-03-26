// Some copyright should be here...

#include "MotivePluginPrivatePCH.h"

DEFINE_LOG_CATEGORY(MotiveLog);

#define LOCTEXT_NAMESPACE "FMotiveModule"

void FMotiveModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(MotiveLog, Warning, TEXT("Motive Module Startup"));
}

void FMotiveModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(MotiveLog, Warning, TEXT("Motive Module Shutdown"));
}


	
IMPLEMENT_MODULE(FMotiveModule, MotivePlugin)

#undef LOCTEXT_NAMESPACE