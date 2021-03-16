/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FTonemapPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FTonemapPS);
	SHADER_USE_PARAMETER_STRUCT(FTonemapPS, FGlobalShader);

	using FPermutationDomain = TonemapperPermutation::FDesktopDomain;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FTonemapParameters, Tonemap)
//#if CELSHADING    
		SHADER_PARAMETER_STRUCT_REF(FSceneTexturesUniformParameters, SceneTextures)
//#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FTonemapCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FTonemapCS);
	SHADER_USE_PARAMETER_STRUCT(FTonemapCS, FGlobalShader);

	using FPermutationDomain = TShaderPermutationDomain<
		TonemapperPermutation::FDesktopDomain,
		TonemapperPermutation::FTonemapperEyeAdaptationDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FTonemapParameters, Tonemap)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, RWOutputTexture)
//#if CELSHADING
		SHADER_PARAMETER_STRUCT_REF(FSceneTexturesUniformParameters, SceneTextures)
//#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


  - FScreenPassTexture AddTonemapPass 함수 밑에 if (bComputePass) 부분 양쪽
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (bComputePass)
	{
		FTonemapCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FTonemapCS::FParameters>();
		PassParameters->Tonemap = CommonParameters;
		PassParameters->RWOutputTexture = GraphBuilder.CreateUAV(Output.Texture);
		FSceneRenderTargets& SceneContext = FSceneRenderTargets::Get(GraphBuilder.RHICmdList);
//#if CELSHADING
		PassParameters->SceneTextures = CreateSceneTextureUniformBuffer(SceneContext, View.FeatureLevel, ESceneTextureSetupMode::All, EUniformBufferUsage::UniformBuffer_MultiFrame);
//#endif
		FTonemapCS::FPermutationDomain PermutationVector;
		PermutationVector.Set<TonemapperPermutation::FDesktopDomain>(DesktopPermutationVector);
		PermutationVector.Set<TonemapperPermutation::FTonemapperEyeAdaptationDim>(bEyeAdaptation);

		TShaderMapRef<FTonemapCS> ComputeShader(View.ShaderMap, PermutationVector);

		FComputeShaderUtils::AddPass(
			GraphBuilder,
			RDG_EVENT_NAME("Tonemap %dx%d (CS GammaOnly=%d)", OutputViewport.Rect.Width(), OutputViewport.Rect.Height(), Inputs.bGammaOnly),
			ComputeShader,
			PassParameters,
			FComputeShaderUtils::GetGroupCount(OutputViewport.Rect.Size(), FIntPoint(GTonemapComputeTileSizeX, GTonemapComputeTileSizeY)));
	}
	else
	{
		FTonemapPS::FParameters* PassParameters = GraphBuilder.AllocParameters<FTonemapPS::FParameters>();
		PassParameters->Tonemap = CommonParameters;
		PassParameters->RenderTargets[0] = Output.GetRenderTargetBinding();
		FSceneRenderTargets& SceneContext = FSceneRenderTargets::Get(GraphBuilder.RHICmdList);
//#if CELSHADING
		PassParameters->SceneTextures = CreateSceneTextureUniformBuffer(SceneContext, View.FeatureLevel, ESceneTextureSetupMode::All, EUniformBufferUsage::UniformBuffer_MultiFrame);
//#endif
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////


  - PostProcessTonemap.usf 의 float4 TonemapCommonPS 맨 끝 부분
//#if CELSHADING
	FScreenSpaceData ScreenSpaceData = GetScreenSpaceData(SceneUV, false);
	half4 Local1 = SceneColor;
	#if FEATURE_LEVEL > FEATURE_LEVEL_ES3_1 // On mobile ExposureScale applied in base pass PS
		Local1.rgb *= ExposureScale;
	#endif
	half3 Local2 = pow(Local1.rgb, InverseGamma.x);

	half4 Local3 = ScreenSpaceData.GBuffer.Depth;
	half4 Local4 = ScreenSpaceData.GBuffer.CustomDepth;
	float Local5 = (Local3.r - Local4.r);
	float Local6 = abs(Local5);
	float Local7 = min(max(Local6,0.00000000),1.00000000);
	float Local8 = ceil(Local7);
	float Local9 = (1.00000000 - Local8);
	half4 Local10 = ScreenSpaceData.GBuffer.CustomStencil;
	float Local11 = (Local10.rgba.r - 1.00000000);
	float Local12 = abs(Local11);
	float Local13 = min(max(Local12,0.00000000),1.00000000);
	float Local14 = (1.00000000 - Local13);
	float Local15 = (Local9 * Local14);

	OutColor.rgb = lerp(OutColor.rgb, Local2.rgb, float(Local15));
//#endif
	return OutColor;
}

// pixel shader entry point
void MainPS(
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

