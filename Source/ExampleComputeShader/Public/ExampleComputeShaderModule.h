// ExampleComputeShaderModule.h
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RHIResources.h" 


class FExampleComputeShaderModule : public IModuleInterface {
	virtual void StartupModule() override;
	/*virtual void ShutdownModule() override;*/
};



class EXAMPLECOMPUTESHADER_API FExampleComputeShaderResource : public FRenderResource // 使用EXAMPLECOMPUTESHADER_API因为我们要在别的模块访问到这个类
{
private:
	static FExampleComputeShaderResource* GInstance; // 单例模式
	FExampleComputeShaderResource() {} // 构造函数私有
	
public:
	FRWBufferStructured InputBuffer;  // RWStructuredBuffer<float> InputBuffer;
	FRWBufferStructured OutputBuffer; // RWStructuredBuffer<float> OutputBuffer;

	// 初始化所有buffer，override基类
	// 此函数由FRenderResource::InitResource(FRHICommandList&)调用
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

	// 此函数由FRenderResource::ReleaseResource(FRHICommandList&)调用
	virtual void ReleaseRHI() override; // 释放所有buffer

	static FExampleComputeShaderResource* Get(); // 单例模式
	

};

// 确保以下函数被导出（添加 EXAMPLECOMPUTESHADER_API 宏）
EXAMPLECOMPUTESHADER_API void DispatchExampleComputeShader_GameThread(const TArray<float>& InputValues, float Scale, float Translate, FExampleComputeShaderResource* Resource);
EXAMPLECOMPUTESHADER_API TArray<float> GetGPUReadback(FExampleComputeShaderResource* Resource, const TArray<float>& OutputVal);