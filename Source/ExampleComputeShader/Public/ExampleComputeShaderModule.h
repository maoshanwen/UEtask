// ExampleComputeShaderModule.h
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RHIResources.h" 


class FExampleComputeShaderModule : public IModuleInterface {
	virtual void StartupModule() override;
	/*virtual void ShutdownModule() override;*/
};



class EXAMPLECOMPUTESHADER_API FExampleComputeShaderResource : public FRenderResource // ʹ��EXAMPLECOMPUTESHADER_API��Ϊ����Ҫ�ڱ��ģ����ʵ������
{
private:
	static FExampleComputeShaderResource* GInstance; // ����ģʽ
	FExampleComputeShaderResource() {} // ���캯��˽��
	
public:
	FRWBufferStructured InputBuffer;  // RWStructuredBuffer<float> InputBuffer;
	FRWBufferStructured OutputBuffer; // RWStructuredBuffer<float> OutputBuffer;

	// ��ʼ������buffer��override����
	// �˺�����FRenderResource::InitResource(FRHICommandList&)����
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

	// �˺�����FRenderResource::ReleaseResource(FRHICommandList&)����
	virtual void ReleaseRHI() override; // �ͷ�����buffer

	static FExampleComputeShaderResource* Get(); // ����ģʽ
	

};

// ȷ�����º�������������� EXAMPLECOMPUTESHADER_API �꣩
EXAMPLECOMPUTESHADER_API void DispatchExampleComputeShader_GameThread(const TArray<float>& InputValues, float Scale, float Translate, FExampleComputeShaderResource* Resource);
EXAMPLECOMPUTESHADER_API TArray<float> GetGPUReadback(FExampleComputeShaderResource* Resource, const TArray<float>& OutputVal);