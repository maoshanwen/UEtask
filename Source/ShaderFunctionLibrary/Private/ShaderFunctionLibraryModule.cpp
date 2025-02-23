// ShaderFunctionLibraryModule.cpp

#include "ShaderFunctionLibraryModule.h"
#include "Modules/ModuleManager.h"
#include "ExampleComputeShaderModule.h"

IMPLEMENT_MODULE(FShaderFunctionLibraryModule, FShaderFunctionLibrary)
void FShaderFunctionLibraryModule::StartupModule()
{
    // 输出日志，确认模块已启动
    UE_LOG(LogTemp, Log, TEXT("ShaderFunctionLibrary Module is starting..."));

    // 检查依赖模块是否加载
    if (!FModuleManager::Get().IsModuleLoaded("ExampleComputeShader"))
    {
        UE_LOG(LogTemp, Error, TEXT("ExampleComputeShader module is not loaded. Initialization failed!"));
        return;
    }

   
}
