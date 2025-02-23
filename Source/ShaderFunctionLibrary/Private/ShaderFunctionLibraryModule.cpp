// ShaderFunctionLibraryModule.cpp

#include "ShaderFunctionLibraryModule.h"
#include "Modules/ModuleManager.h"
#include "ExampleComputeShaderModule.h"

IMPLEMENT_MODULE(FShaderFunctionLibraryModule, FShaderFunctionLibrary)
void FShaderFunctionLibraryModule::StartupModule()
{
    // �����־��ȷ��ģ��������
    UE_LOG(LogTemp, Log, TEXT("ShaderFunctionLibrary Module is starting..."));

    // �������ģ���Ƿ����
    if (!FModuleManager::Get().IsModuleLoaded("ExampleComputeShader"))
    {
        UE_LOG(LogTemp, Error, TEXT("ExampleComputeShader module is not loaded. Initialization failed!"));
        return;
    }

   
}
