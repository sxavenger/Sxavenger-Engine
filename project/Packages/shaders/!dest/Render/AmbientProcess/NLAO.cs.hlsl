//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "AmbientProcess.hlsli"
#include "NLAO.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gMaterial_AO : register(u0); //!< material and ao texture

Texture2D<float4> gPosition : register(t0); //!< position texture
Texture2D<float4> gNormal   : register(t1); //!< normal texture
Texture2D<float4> gDepth    : register(t2); //!< depth texture

////////////////////////////////////////////////////////////////////////////////////////////
// InfomationSurface structure
////////////////////////////////////////////////////////////////////////////////////////////
struct InfomationSurface {
	
	//* member *//
	
	float3 position;
	float3 normal;
	float  depth;
	
	//* method *//
	
	bool GetSurface(uint2 index) {

		depth = gDepth.Load(int3(index, 0)).r;
		
		if (depth == 1.0f) {
			return false;
		}
		
		position = gPosition.Load(int3(index, 0)).xyz;
		normal = gNormal.Load(int3(index, 0)).xyz * 2.0f - 1.0f;
		
		return true;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessOutput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct ProcessOutput {
	
	//* member *//
	
	float ao;
	
	//* method *//
	
	void SetOutput(uint2 index) {
		gMaterial_AO[index].a = ao;
	}
	
	void Exception(uint2 index) {
		gMaterial_AO[index].a = 1.0f;
	}
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// common method
////////////////////////////////////////////////////////////////////////////////////////////

bool CheckOverTexture(uint2 index) {
	return any(index >= gConfig.size);
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
	
	ProcessOutput output = (ProcessOutput)0;

	InfomationSurface surface;
	
	if (!surface.GetSurface(index)) {
		output.Exception(index);
		return;
	}

	static const int2 kSamples = int2(gParameter.samples.x, gParameter.samples.y);
	
	float sum   = 0.0f;
	float count = 0.0f;
	
	for (int y = -kSamples.y; y <= kSamples.y; ++y) {
		for (int x = -kSamples.x; x <= kSamples.x; ++x) {

			if (x == 0 && y == 0) {
				continue;
			}
			
			int2 sample_index = int2(index) + int2(x, y);
			// hack: uvとかでサンプリングするようにする
			
			InfomationSurface sample_surface;
			
			if (!sample_surface.GetSurface(sample_index)) {
				continue;
			}
			
			float3 v = sample_surface.position - surface.position;
			
			if (dot(v, surface.normal) > 0.0f) {
				continue; //!< sample faceがnormalよりも奥にある
			}
			
			count += 1.0f;
			
			if (length(v) > gParameter.radius) {
				continue; //!< sample faceとの距離がradiusよりも大きい
			}
			
			float l = saturate(dot(sample_surface.normal, surface.normal));
			// note: aoの計算的には, 1.0f - l でもいい.
			sum += l;
		}
	}
	
	output.ao = saturate(sum / count) * gParameter.strength;
	output.SetOutput(index);
	
}
