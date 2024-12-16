//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "XclipseProcess.hlsli"
#include "../../Camera3d.hlsli"

//=========================================================================================
// Input
//=========================================================================================

Texture2D<float4> gInput : register(t0); //!< input xclipse texture

Texture2D<float4> gNormal   : register(t1); //!< systematic normal texture
Texture2D<float4> gPosition : register(t2); //!< systematic normal texture
Texture2D<float>  gDepth    : register(t3);  //!< depth texture

ConstantBuffer<Camera3d> gCamera : register(b0); //!< camera buffer

//=========================================================================================
// Output
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0); //!< output xclipse texture

////////////////////////////////////////////////////////////////////////////////////////////
// static const variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kKernelSize    = 64; //!< kernel size
static const float kKernelRadius = 0.2f; //!< kernel radius

static const float kMinDistance = 0.1f; //!< minimum distance
static const float kMaxDistance = 1.0f; //!< maximum distance

static const float kStrength = 1.0f; //!< occulusion strength

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float FixedDepth(float depth) {
	float z = depth;
	// 逆射影行列を使用してビュー空間の位置に戻す
	float4 clipSpacePos = float4(0.0, 0.0, z, 1.0);
	float4 viewSpacePos = mul(clipSpacePos, gCamera.projInverseMatrix);
	
	// viewSpacePos.z / viewSpacePos.w で線形深度を取得
	return viewSpacePos.z / viewSpacePos.w;
}

float3 GeneratePseudoRandom(uint2 currentIndex) { //!< test random function
	uint seed = currentIndex.x * 0x1f1f1f1f + currentIndex.y;
	seed = (seed << 13) ^ seed;
	return normalize(float3(
		2.0f * fmod(float(seed), 256.0f) / 256.0f - 1.0f,
		2.0f * fmod(float(seed >> 8), 256.0f) / 256.0f - 1.0f,
		0.0f
	));
}

float3 GetViewPosition(uint2 currentIndex) {
	float2 uv = (currentIndex + 0.5f) / gConfig.size;
	float2 ndc = uv * 2.0f - 1.0f;
	
	float4 clipSpacePos = float4(ndc, gDepth[currentIndex].r, 1.0f);
	float4 viewSpacePos = mul(clipSpacePos, gCamera.projInverseMatrix);
	
	return viewSpacePos.xyz / viewSpacePos.w;
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
	
	//float4 color = gInput[currentIndex];
	
	//if (color.a == 0.0f) {
	//	return;
	//}
	
	float3 normal = normalize(gNormal[currentIndex].xyz + 1.0f) * 0.5f;
	float3 viewPosition = GetViewPosition(currentIndex);
	float  depth    = FixedDepth(gDepth[currentIndex].r);
	
	//float3 random = GeneratePseudoRandom(currentIndex);
	float3 random = float3(1.0f, 1.0f, 1.0f);
	
	float3 tangent   = normalize(random - normal * dot(random, normal));
	float3 bitangent = cross(normal, tangent);
	
	float3x3 kernelMatrix = float3x3(
		tangent,
		bitangent,
		normal
	); //!< kernel matrix
	
	float occlusion = 0.0f;
	
	for (uint i = 0; i < kKernelSize; ++i) {
		float3 sampleVector = mul(GeneratePseudoRandom(uint2(i, i)), kernelMatrix);
		float3 samplePoint    = viewPosition + sampleVector * kKernelRadius;
		float4 samplePointNDC = mul(float4(samplePoint, 1.0), gCamera.projMatrix);
		samplePointNDC /= samplePointNDC.w;
		
		float2 samplePointUV = (samplePointNDC.xy + 1.0f) * 0.5f;
		uint2  samplePointIndex = uint2(samplePointUV * size);
		
		float depthSample = FixedDepth(gDepth[samplePointIndex].r);
		float delta       = depth - depthSample;
		
		if (delta > kMinDistance && delta < kMaxDistance) {
			occlusion += 1.0f;
		}
	}
	
	occlusion = clamp(occlusion / kKernelSize, 0.0f, 1.0f);
	float3 col = float3(1.0f - occlusion * kStrength, 1.0f - occlusion * kStrength, 1.0f - occlusion * kStrength);

	gOutput[currentIndex] = float4(col, 1.0f);

	//float3 final = lerp(gInput[currentIndex].rgb, col, occlusion);
	//gOutput[currentIndex] = float4(final, 1.0f);
	
}
