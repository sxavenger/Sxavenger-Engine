//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../Camera.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0); //!< input texture
RWTexture2D<float4> gOutput : register(u1); //!< output texture

ConstantBuffer<Camera> gCamera : register(b0);

struct Parameter {
	uint2 strength;
	float focus;
	float f;
	uint isDebugView;
};
ConstantBuffer<Parameter> gParameter : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// common variables
////////////////////////////////////////////////////////////////////////////////////////////

static const float pi_v = 3.14159265359f;
static const float d    = 0.0001f; //!< 0割対策

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

float FixLinearDepth(float depth) {
	// 逆射影行列を使用してビュー空間の位置に戻す
	float4 clipSpacePos = float4(0.0f, 0.0f, depth, 1.0f);
	float4 viewSpacePos = mul(clipSpacePos, gCamera.projInv);

	// viewSpacePos.z / viewSpacePos.w で線形深度を取得
	return viewSpacePos.z / viewSpacePos.w;
}

float Gaussian2d(float2 diff, float sigma) {
	float a = 1.0f / (2.0f * pi_v * sigma * sigma);
	float b = exp(-(diff.x * diff.x + diff.y * diff.y) / (2.0f * sigma * sigma));
	return a * b;
}

float4 Sample(uint2 index, int2 diff) {
	int2 texIndex = int2(index) + diff;
	texIndex = clamp(texIndex, float2(0.0f, 0.0f), gConfig.size - 1.0f);
	return gInput[texIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	Surface surface;
	surface.GetSurface(index);

	float depth = max(FixLinearDepth(surface.depth), d);
	
	float focus = gParameter.focus;
	float f     = max(gParameter.f, d);
	
	// ピントからの相対距離を計算
	float coc = abs(depth - focus) / depth;
	
	float factor = saturate(abs(depth - focus) / focus);
	
	float radius = coc * (focus / f);
	radius *= lerp(1.0, 24.0f, pow(factor, 2.0)); // 距離に応じてぼかしを増幅
	
	float4 color     = (float4)0;
	float weightSum = 0;

	const int2 diff = int2(gParameter.strength);
	
	for (int x = -diff.x; x <= diff.x; ++x) {
		for (int y = -diff.y; y <= diff.y; ++y) {
			
			float weight = Gaussian2d(float2(x, y), radius);
			color       += weight * Sample(index, int2(x, y));
			weightSum   += weight;

		}
	}

	color = saturate(color / weightSum);

	if (gParameter.isDebugView) {
		float alpha    = gInput[index].a;
		gOutput[index] = float4(saturate(radius), saturate(radius), saturate(radius), alpha);
		return;
	}
	
	gOutput[index] = color;
}
