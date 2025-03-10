// ExampleGraphicsShaderModule.h
#pragma once
#include "Math/Matrix.h" // 引入矩阵库
#include "Modules/ModuleManager.h"

class FExampleGraphicsShaderModule : public IModuleInterface {
	virtual void StartupModule() override;
	// virtual void ShutdownModule() override;
};





// 这个struct对应hlsl里面的VertexAttributes
EXAMPLEGRAPHICSSHADER_API struct VertexAttributes
{
	
	FVector4f Position; // Normally we use homogeneous coordinate so we declared Vec4 but here for demonstration we only use first 2 components to store NDC
	FVector4f Color; // RGBA
	FVector4f Normal;   // 顶点法向量
	FVector4f Trans_time;
	FMatrix44f Transform;   // 变换矩阵（平移、旋转、缩放） // 变换矩阵（平移、旋转、缩放）
	
};


// 存VS和PS对应的渲染资源
class EXAMPLEGRAPHICSSHADER_API FExampleGraphicsShaderResource : public FRenderResource
{
	static FExampleGraphicsShaderResource* GInstance; // Singleton instance
public:
	
	FVector4f TColor;
	TArray<VertexAttributes> Vertices; // 用于存储顶点数据
	
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
	virtual void ReleaseRHI() override;

	FVertexDeclarationRHIRef VertexDeclarationRHI; // 定义vertex的数据是如何存储在buffer的。见InitRHI()
	FReadBuffer VertexBuffer; // GPU只读不写，所以定义为ReadBuffer
	static FExampleGraphicsShaderResource* Get(); // Singleton instance
};

EXAMPLEGRAPHICSSHADER_API void RenderExampleGraphicsShader_GameThread(UTextureRenderTarget2D* TextureRenderTarget2D, FExampleGraphicsShaderResource* Resource, const FVector4f& TColor, const FMatrix44f& ViewMatrix, const FMatrix44f& ProjectionMatrix);
