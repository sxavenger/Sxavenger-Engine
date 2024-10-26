//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "XclipseProcess.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// Input
//=========================================================================================

ConstantBuffer<Camera> gCamera : register(b0);

//=========================================================================================
// Output
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// static const variables
////////////////////////////////////////////////////////////////////////////////////////////

//* math
static const float pi_v = 3.14159265f;

static const float3 kWavelength = float3(0.68f, 0.55f, 0.44f); //!< 波長の逆数(赤、緑、青チャンネル)
static const float3 kInvWavelength = 1.0f / pow(kWavelength, 4.0f);


static const float kInnerRadius  = 8.0f;          //!< 内側の（惑星の）半径（地球半径6371km）
static const float kOuterRadius  = kInnerRadius + 20.0f; //!< 外側の（大気の）半径（地球半径6371km + 大気の厚さ100km）
static const float kInnerRadiusPow2 = pow(kInnerRadius, 2.0f);
static const float kOuterRadiusPow2 = pow(kOuterRadius, 2.0f);

static const float kScale      = 1.0f / (kOuterRadius - kInnerRadius);
static const float kScaleDepth = 0.25f; //!< 大気の平均密度が見つかる高度
static const float kScaleOverScaleDepth = kScale / kScaleDepth;

//* enviorment 
static const float kKr   = 0.0025f; //!< レイリー散乱係数
static const float kKm   = 0.001f; //!< ミー散乱係数
static const float kESun = 100.0f;   //!< 太陽の強さ

static const float kKrESun = kKr * kESun;
static const float kKmESun = kKm * kESun;
static const float kKr4pi  = kKr * pi_v * 4.0f;
static const float kKm4pi  = kKm * pi_v * 4.0f;

static const float kG = -0.70f; //!< ミー散乱の位相関数のパラメーター

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float3 IntersectionPos(float3 rayPos, float3 rayDir, float sphereRadius) { //!< 指定された半径の球体（ここでは大気）の表面とカメラからのレイが交差する点を計算
	const float A = dot(rayDir, rayDir);
	const float B = 2.0 * dot(rayPos, rayDir);
	const float C = dot(rayPos, rayPos) - sphereRadius * sphereRadius;

	float discriminant = B * B - 4.0 * A * C;
	if (discriminant < 0.0f)
		return float3(0, 0, 0); // 交差なし

	float t = (-B - sqrt(discriminant)) / (2.0 * A);
	return rayPos + rayDir * t;
}

float IntegralApproximation(float fCos) { //!< 散乱の積分を近似する関数
	float x = 1.0 - fCos;
	return kScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.8 + x * 5.25))));
}

////////////////////////////////////////////////////////////////////////////////////////////
// frustum ray methods
////////////////////////////////////////////////////////////////////////////////////////////

struct FrustumRay {
	float3 leftTop;
	float3 leftBottom;
	float3 rightTop;
};

FrustumRay CalculateFrustumRay(Camera camera) {

	// クリップ空間でのコーナー座標を設定
	float4 cornerTL = float4(-1.0f,  1.0f, 0.0f, 1.0f); // 左上
	float4 cornerTR = float4( 1.0f,  1.0f, 0.0f, 1.0f); // 右上
	float4 cornerBL = float4(-1.0f, -1.0f, 0.0f, 1.0f); // 左下

	// クリップ空間からワールド空間への変換
	float4 worldTL = mul(cornerTL, camera.projInverseMatrix);
	float4 worldTR = mul(cornerTR, camera.projInverseMatrix);
	float4 worldBL = mul(cornerBL, camera.projInverseMatrix);

	// w で正規化
	worldTL /= worldTL.w;
	worldTR /= worldTR.w;
	worldBL /= worldBL.w;

	// カメラのワールド空間での位置
	float3 cameraPos = camera.position.xyz;

	FrustumRay result;
	
	// 視錐体のレイ方向を計算
	result.leftTop    = normalize(worldTL.xyz - cameraPos);
	result.rightTop   = normalize(worldTR.xyz - cameraPos);
	result.leftBottom = normalize(worldBL.xyz - cameraPos);

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUMTHREADS_X, _NUMTHREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 currentIndex = dispatchThreadId.xy;
	uint2 size         = gConfig.size;

	// 画像サイズ以上の場合, 書き込みをしない
	if (any(currentIndex >= size)) {
		return;
	}

	// 現在までのcolorの取り出し
	float4 color = gOutput[currentIndex];

	// pixelにすでに描画されているかの判定
	if (color.a != 0.0f) {
		return; //!< されていた場合, そのpixel colorを優先
		// HACK: depth情報で1だったらでもいいかも
	}

	float2 uv = (float2(currentIndex) / size) * 2.0f - 1.0f;
	
	Camera camera = gCamera;

	//* 大気散乱シェーダー *//

	// TEST VALUE
	
	float exposure = 1.0f;
	float3 v3LightDir = normalize(float3(0.0f, -1.0f, 0.0f));

	// rayを求める

	float4 viewPos = mul(float4(-uv.x, uv.y, 1.0f, 1.0f), camera.projInverseMatrix);
	//viewPos /= viewPos.w; // 透視除算

	// ビュー空間からワールド空間に変換して最終位置を得る
	float3 ray = mul(float4(viewPos.xyz, 0.0f), camera.worldMatrix).xyz; // 行行列なので左側に掛ける

	//float3 nor = (ray + 1.0f) / 2.0f;
	//gOutput[currentIndex] = float4(nor, 1.0f);
	//return;
	

	float3 cameraPos = camera.position.xyz;
	float3 skyPos = IntersectionPos(cameraPos, ray, kOuterRadius);

	if (skyPos.x == 0.0f) {
		// カメラが球の外にある場合は描画を中止
		gOutput[currentIndex] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}

	// 大気散乱のための主要な計算
	float3 invWavelength = kInvWavelength;
	float3 v3Pos         = skyPos;
	float3 v3Ray         = v3Pos - cameraPos;
	float fFar           = length(v3Ray);
	v3Ray /= fFar;

	float3 v3Start     = cameraPos;
	float fHeight      = length(v3Start);
	float fDepth       = exp(kScaleOverScaleDepth * (kInnerRadius - fHeight)); //!< 仮でyを入れておく
	float fStartAngle  = dot(v3Ray, v3Start) / fHeight;
	float fStartOffset = fDepth * IntegralApproximation(fStartAngle);

	const int nSamples   = 5;
	const float fSamples = nSamples;
	float fSampleLength  = fFar / fSamples;
	float fScaledLength  = fSampleLength * kScale;
	float3 v3SampleRay   = v3Ray * fSampleLength;
	float3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	float3 v3FrontColor = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < nSamples; ++i) {
		float fHeightF      = length(v3SamplePoint);
		float fDepthF       = exp(kScaleOverScaleDepth * (kInnerRadius - fHeightF));
		
		float fLightAngleF  = dot(v3LightDir, v3SamplePoint) / fHeightF;
		float fCameraAngleF = dot(v3Ray, v3SamplePoint) / fHeightF;

		float lightAngleS  = IntegralApproximation(fLightAngleF);
		float cameraAngleS = IntegralApproximation(fCameraAngleF);
		
		float fScatterF     = (fStartOffset + fDepthF * (lightAngleS - cameraAngleS));
		float3 v3Attenuate = exp(-fScatterF * (invWavelength * kKr4pi + kKm4pi));
		v3FrontColor      += v3Attenuate * (fDepthF * fScaledLength);
		v3SamplePoint     += v3SampleRay;
	}

	float4 primaryColor   = float4(v3FrontColor * (invWavelength * kKrESun), 1.0);
	float4 secondaryColor = float4(v3FrontColor * kKmESun, 1.0);

	float3 v3Direction    = cameraPos - v3Pos;
	float fCos            = dot(v3LightDir, v3Direction) / length(v3Direction);
	float fRayPhase       = 0.75 * (1.0 + fCos * fCos);

	const float g2  = kG * kG;
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos * fCos) / pow(abs(1.0 + g2 - 2.0 * kG * fCos), 1.5);
    
	float3 raycolor = (primaryColor * fRayPhase).xyz;
	float3 miecolor = (secondaryColor * fMiePhase).xyz;

	float3 c = float3(1.0, 1.0, 1.0) - exp(-exposure * (raycolor + miecolor));

	if (isnan(c.r) || isnan(c.g) || isnan(c.b)) {
		gOutput[currentIndex] = float4(0.0f, 1.0f, 0.0f, 1.0f);
		return;
	}

	color = float4(saturate(c), 1.0f);
	gOutput[currentIndex] = color;

}
