// ExampleGraphicsShaderModule.cpp
#include "ExampleGraphicsShaderModule.h"

#include "ClearQuad.h"
#include "SelectionSet.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Logging/LogMacros.h" // 用于日志记录


IMPLEMENT_MODULE(FExampleGraphicsShaderModule, ExampleGraphicsShader)

void FExampleGraphicsShaderModule::StartupModule()
{
	// 在模块初始化后将项目目录的Shaders文件夹Mount到/MyGraphicsShader
	AddShaderSourceDirectoryMapping(TEXT("/MyGraphicsShader"), FPaths::ProjectDir() / TEXT("Shaders"));
}

class FExampleGraphcisShaderVS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FExampleGraphcisShaderVS, EXAMPLEGRAPHICSSHADER_API);
	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}
	FExampleGraphcisShaderVS() {}
	FExampleGraphcisShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer) {
		ViewMatrix.Bind(Initializer.ParameterMap, TEXT("ViewMatrix"));
		ProjectionMatrix.Bind(Initializer.ParameterMap, TEXT("ProjectionMatrix"));
	}

	void SetParameters(FRHICommandList& RHICmdList, const  FMatrix44f& View, const FMatrix44f& Projection) {
		
		

		// 设置视图矩阵
		ViewMatrix.Value(RHICmdList, View);

		// 设置投影矩阵
		ProjectionMatrix.SetValue(RHICmdList, Projection);
	}

private:
	LAYOUT_FIELD(FShaderParameter, ViewMatrix);
	LAYOUT_FIELD(FShaderParameter, ProjectionMatrix);

};

class FExampleGraphcisShaderPS : public FGlobalShader
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FExampleGraphcisShaderPS, EXAMPLEGRAPHICSSHADER_API);
	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}
};


IMPLEMENT_SHADER_TYPE(, FExampleGraphcisShaderVS, TEXT("/MyGraphicsShader/ExampleGraphicsShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FExampleGraphcisShaderPS, TEXT("/MyGraphicsShader/ExampleGraphicsShader.usf"), TEXT("MainPS"), SF_Pixel);



FExampleGraphicsShaderResource* FExampleGraphicsShaderResource::GInstance = nullptr;
FExampleGraphicsShaderResource* FExampleGraphicsShaderResource::Get()
{
	if (GInstance == nullptr)
	{
		GInstance = new FExampleGraphicsShaderResource();
		ENQUEUE_RENDER_COMMAND(FInitExampleGraphicsShaderResource)([](FRHICommandList& RHICmdList)
			{
				GInstance->InitResource(RHICmdList);
			});
	}
	return GInstance;
}

void FExampleGraphicsShaderResource::InitRHI(FRHICommandListBase& RHICmdList)
{
	// 我们先hard code顶点数据
	TArray<VertexAttributes> Vertices_t;

	float CurrentTime = FPlatformTime::Seconds(); // 获取当前时间
	
	FVector4f TimeParam1 = FVector4f(CurrentTime, 0.0f, 0.0f, 0.0f); // 将时间放在 x 轴，其他分量为 0
	UE_LOG(LogTemp, Log, TEXT("Current Time in FVector4f: (%f, %f, %f, %f)"),
		TimeParam1.X, TimeParam1.Y, TimeParam1.Z, TimeParam1.W);


	FVector4f BaseColor = TColor; // 使用输入的基础颜色
	

	FMatrix44f TranslationMatrix = FMatrix44f::Identity; // 初始化为单位矩阵
	TranslationMatrix.M[3][0] = 0.2f; // 设置平移 X
	TranslationMatrix.M[1][3] = 0.0f; // 设置平移 Y
	TranslationMatrix.M[2][3] = 0.0f; // 设置平移 Z

	FMatrix44f RotationMatrix = FMatrix44f::Identity; // 初始化为单位矩阵
	float AngleInRadians = FMath::DegreesToRadians(3.0f); // 将角度转换为弧度
	RotationMatrix.M[0][0] = FMath::Cos(AngleInRadians); // 第一行，第一列
	RotationMatrix.M[0][1] = FMath::Sin(AngleInRadians); // 第一行，第二列
	RotationMatrix.M[1][0] = -FMath::Sin(AngleInRadians); // 第二行，第一列
	RotationMatrix.M[1][1] = FMath::Cos(AngleInRadians); // 第二行，第二列

	// 右侧的元素设置为 0，同时设置最后一行的元素为 0 和 1
	RotationMatrix.M[2][0] = 0.0f; // 第三行，第一列
	RotationMatrix.M[2][1] = 0.0f; // 第三行，第二列
	RotationMatrix.M[2][2] = 1.0f; // 第三行，第三列
	RotationMatrix.M[3][3] = 1.0f; // 最后一行最后一列


	FMatrix44f ScalingMatrix = FMatrix44f::Identity; // 初始化为单位矩阵
	ScalingMatrix.M[0][0] = 0.5f; // X 轴缩放
	ScalingMatrix.M[1][1] = 0.5f; // Y 轴缩放
	ScalingMatrix.M[2][2] = 0.5f; // Z 轴缩放


	// 如果需要组合矩阵，按照顺序进行乘法
	FMatrix44f TransformMatrix = TranslationMatrix* ScalingMatrix;


	// 创建三个不同的顶点颜色，这里我们通过调整 RGB 分量来实现
	// 创建三个不同的顶点颜色，分别加重红色、绿色和蓝色
	FVector4f Color1 = FVector4f(FMath::Clamp(BaseColor.X + 0.9f, 0.0f, 1.0f), BaseColor.Y, BaseColor.Z, BaseColor.W); // 加重红色
	FVector4f Color2 = FVector4f(BaseColor.X, FMath::Clamp(BaseColor.Y + 0.9f, 0.0f, 1.0f), BaseColor.Z, BaseColor.W); // 加重绿色
	FVector4f Color3 = FVector4f(BaseColor.X, BaseColor.Y, FMath::Clamp(BaseColor.Z + 0.9f, 0.0f, 1.0f), BaseColor.W); // 加重蓝色
	
	FVector4f NormalVector = FVector4f(0, 0, 1, 0); // 默认法向量
	Vertices_t.Add({FVector4f(-1, 1, 1, 1), Color1, NormalVector , TimeParam1, TransformMatrix });
	Vertices_t.Add({FVector4f(1, 1, 1, 1), Color2, NormalVector , TimeParam1, TransformMatrix });
	Vertices_t.Add({FVector4f(-1, -1, 1, 1), Color1, NormalVector , TimeParam1, TransformMatrix });


	// 初始化buffer并拷贝
	uint32 NumBytes = sizeof(VertexAttributes) * Vertices_t.Num();
	FRHIResourceCreateInfo CreateInfo(TEXT("VertexBuffer"));
	VertexBuffer.Buffer = RHICmdList.CreateVertexBuffer(NumBytes, BUF_Static, CreateInfo);
	VertexAttributes* GPUBufferPtr = static_cast<VertexAttributes*>(RHICmdList.LockBuffer(VertexBuffer.Buffer, 0, sizeof(VertexAttributes) * Vertices_t.Num(), RLM_WriteOnly));
	FMemory::Memcpy(GPUBufferPtr, Vertices_t.GetData(), NumBytes);
	RHICmdList.UnlockBuffer(VertexBuffer.Buffer);
	// 到此为止，我们的buffer存储了3个vertices，每个vertex有4个float作为position和4个float作为color。所以一个vertex是4*sizeof(float) + 4*sizeof(float)==32字节。当然3个vertices总共96个字节。

	// 下面我们定义vertex buffer的数据排布。

	// 定义VertexDeclaration
	uint16 Stride = sizeof(VertexAttributes);
	FVertexDeclarationElementList Elements;
	

	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Position), VET_Float4, 0, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Color), VET_Float4, 1, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Normal), VET_Float4, 2, Stride)); // 添加法向量元素
	// 为变换矩阵的每一行定义元素
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Trans_time), VET_Float4, 3, Stride)); // 添加时间参数
	

	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Transform.M[0]), VET_Float4, 4, Stride)); // 第一行
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Transform.M[1]), VET_Float4, 5, Stride)); // 第二行
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Transform.M[2]), VET_Float4, 6, Stride)); // 第三行
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(VertexAttributes, Transform.M[3]), VET_Float4, 7, Stride)); // 第四行
	
	VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	// 根据vertex的定义，前四个float是position，并且我们在hlsl将其定义为ATTRIBUTE0。所以它的InOffset应该是0（因为是结构体的第一个成员）；stride应该是sizeof(VertexAttributes)因为当前指针到下一个vertex指针的偏移量是sizeof(VertexAttributes)
	// Color同理，但它的InOffset是Position的大小，即4*sizeof(float)
	// 如果读者熟悉opengl API，FVertexElement与glVertexAttribPointer相似

	this->Vertices = Vertices_t; // 将临时数组赋值给类的成员变量

	// 打印变换矩阵
	UE_LOG(LogTemp, Log, TEXT("Transform Matrix:"));
	for (int32 i = 0; i < 4; i++)
	{
		UE_LOG(LogTemp, Log, TEXT("%f %f %f %f"),
			TransformMatrix.M[i][0],
			TransformMatrix.M[i][1],
			TransformMatrix.M[i][2],
			TransformMatrix.M[i][3]);
	}

	
}

void FExampleGraphicsShaderResource::ReleaseRHI()
{
	if (VertexBuffer.Buffer)
	{
		VertexBuffer.Release();
	}
	VertexDeclarationRHI.SafeRelease();
}

void RenderExampleGraphicsShader_RenderThread(FRHICommandList& RHICmdList, FExampleGraphicsShaderResource* Resource, FRHITexture* RenderTarget, const FVector4f& Color, const FMatrix44f& ViewMatrix, const FMatrix44f& ProjectionMatrix)
{
	Resource->TColor = Color;
	// 设置uniform变量
	
	// 获取当前时间并更新时间参数
	float CurrentTime = FPlatformTime::Seconds();
	FVector4f TimeParam1 = FVector4f(CurrentTime, 0.0f, 0.0f, 0.0f);
	UE_LOG(LogTemp, Log, TEXT(" render Current Time: %f"), CurrentTime);
	// 更新每个顶点的时间参数
	uint32 NumVertices = Resource->Vertices.Num();
	for (uint32 i = 0; i < NumVertices; ++i)
	{
		Resource->Vertices[i].Trans_time = TimeParam1; // 更新每个顶点的时间参数
	}

	// 更新顶点缓冲区
	uint32 NumBytes = sizeof(VertexAttributes) * Resource->Vertices.Num();
	VertexAttributes* GPUBufferPtr = static_cast<VertexAttributes*>(RHICmdList.LockBuffer(Resource->VertexBuffer.Buffer, 0, NumBytes, RLM_WriteOnly));
	FMemory::Memcpy(GPUBufferPtr, Resource->Vertices.GetData(), NumBytes); // 将更新后的数据拷贝到GPU
	RHICmdList.UnlockBuffer(Resource->VertexBuffer.Buffer);
	// 调用shader进行渲染
	// 与compute shader不同，graphics shader需要我们定义：1）一个或多个Render Target；2）如何处理Rasterizer、Blend、Depth和Stencil的行为
	// 所以需要给到的初始化参数比compute shader多
	// 一个很好的例子是DrawClearQuad函数，里面详细写了从RHICmdList.BeginRenderPass到RHICmdList.EndRenderPass的所有步骤。如果你的shader无法工作，请使用DrawClearQuad函数进行测试。
	FRHIRenderPassInfo RPInfo(RenderTarget, ERenderTargetActions::Clear_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("ExampleGraphicsShaderRenderPass"));
	// 如果需要测试，可以在这里调用DrawClearQuad(RHICmdList, true, FLinearColor(FVector4(1, 0, 1, 1)), true, 1.0, true, 0);
	// 同时注释其他代码。
	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	TShaderMapRef<FExampleGraphcisShaderVS> VertexShader(ShaderMap);
	TShaderMapRef<FExampleGraphcisShaderPS> PixelShader(ShaderMap);

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<true, CF_Always>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = Resource->VertexDeclarationRHI;

	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

	//VertexShader->SetParameters(RHICmdList, ViewMatrix, ProjectionMatrix);
	// 将 FMatrix44f 转换为 FMatrix（双精度）
	


	// 将转换后的矩阵传递给着色器
	VertexShader->SetParameters(RHICmdList, ViewMatrix, ProjectionMatrix);
	RHICmdList.SetStreamSource(0, Resource->VertexBuffer.Buffer, 0);
	RHICmdList.DrawPrimitive(0, 1, 1);
	RHICmdList.EndRenderPass();
}

void RenderExampleGraphicsShader_GameThread(UTextureRenderTarget2D* TextureRenderTarget2D, FExampleGraphicsShaderResource* Resource, const FVector4f& TColor, const FMatrix44f& ViewMatrix, const FMatrix44f& ProjectionMatrix)
{
	ENQUEUE_RENDER_COMMAND(FRenderExampleGraphicsShader)([Resource, TextureRenderTarget2D, TColor, ViewMatrix, ProjectionMatrix](FRHICommandList& RHICmdList)
		{
			UE_LOG(LogTemp, Log, TEXT("Rendering Frame...")); // 确认有调用
			float CurrentTime = FPlatformTime::Seconds();
			UE_LOG(LogTemp, Log, TEXT("Rendering Frame...game Current Time: %f"), CurrentTime); // 在这里输出时间

			// 调用渲染函数，并传递视图矩阵和投影矩阵
			RenderExampleGraphicsShader_RenderThread(RHICmdList, Resource, TextureRenderTarget2D->GetResource()->GetTexture2DRHI(), TColor, ViewMatrix, ProjectionMatrix);
		});
}
