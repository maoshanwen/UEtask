// ShaderFunctionLibraryModule.h
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExampleComputeShaderModule.h"
#include "ExampleGraphicsShaderModule.h" 
#include "ShaderFunctionLibraryModule.generated.h"



class FShaderFunctionLibraryModule : public IModuleInterface {
	virtual void StartupModule() override;
	// virtual void ShutdownModule() override;
};

UCLASS(meta = (ScriptName = "ShaderFunctionLibrary"), MinimalAPI)

class UShaderFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute ExampleComputeShader"), Category = "My Shader Functions")
	static SHADERFUNCTIONLIBRARY_API TArray<float> ExecuteExampleComputeShader(const TArray<float>& InputVals, float Scale, float Translate) {
		TArray<float> OutputVals;
		DispatchExampleComputeShader_GameThread(InputVals, Scale, Translate, FExampleComputeShaderResource::Get());
		return GetGPUReadback(FExampleComputeShaderResource::Get(), OutputVals);
	}


	/*UFUNCTION(BlueprintCallable, meta = (DisplayName = "Render ExampleGraphicsShader"), Category = "My Shader Functions")
	static SHADERFUNCTIONLIBRARY_API void RenderExampleGraphicsShader(UTextureRenderTarget2D* RenderTarget)
	{
		FExampleGraphicsShaderResource::Get();
		RenderExampleGraphicsShader_GameThread(RenderTarget, FExampleGraphicsShaderResource::Get());
	}*/

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Render ExampleGraphicsShader with Color"), Category = "My Shader Functions")
	static SHADERFUNCTIONLIBRARY_API void RenderExampleGraphicsShader(UTextureRenderTarget2D* RenderTarget, const FVector4f& Color, const FMatrix44f& ViewMatrix, const FMatrix44f& ProjectionMatrix)
	{
		
		FExampleGraphicsShaderResource::Get();
		RenderExampleGraphicsShader_GameThread(RenderTarget, FExampleGraphicsShaderResource::Get(), Color, ViewMatrix, ProjectionMatrix);
	}

};