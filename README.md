# openchat
카톡 오픈챗 자료링크

# 3월 16일 
[언리얼 툰쉐이딩 사전작업 - 특정 오브젝트 톤맵 거치지 않고 디퓨즈 색상 그대로 렌더링 하기]
 - 커스텀 스텐실 1인 오브젝트는 톤맵필터를 거치지 않게 구현함
 - 아래 소스 #if CELSHADING 부분 참조
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


# 3월 11일

[언리얼 툰쉐이딩](https://qiita.com/ruyo/items/28255f26725a6b6bd475)
[서브스턴스 수채화 느낌 배경](https://youtu.be/KPFep7SFIps)

# 2월 13일

[언리얼 툰쉐이딩](https://forums.unrealengine.com/development-discussion/rendering/1537277-toon-shading-models-stylized-rendering-experiments)

# 2월 12일

[블렌더 애드온 Weight Table](https://youtu.be/Q1yPAra_Bes)


# 2월 11일

[언리얼 메타 휴먼](https://youtu.be/_mpof3QZcJ8)

[사진기반 3d모델 생성](https://colab.research.google.com/drive/11z58bl3meSzo6kFqkahMa35G5jmh2Wgt)

[포토샵 클리핑 마스크](https://gkkl.tistory.com/70)

# 2월 10일

[원신 MMD 모델 분석해설 (블렌더)](https://youtu.be/ZRkpA7KPGk4)

[제플린](https://zeplin.io/)

# 2월 9일

[Paint it, Black](https://book.naver.com/bookdb/book_detail.nhn?bid=17848814) (한국 특별 양면 커버판) 요무 스타킹 화보 정발

# 2월 7일

유니티 AAB 젠킨스 세팅 (http://lancekun.com/blog/?p=613)
