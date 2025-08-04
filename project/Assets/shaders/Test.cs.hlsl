//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../../Packages/shaders/Library/Math.hlsli"
#include "../../Packages/shaders/Library/Hammersley.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gNormal   : register(t0);
Texture2D<float4> gPosition : register(t1);

cbuffer Dimension : register(b0) {
	uint2 size;
};

struct Line {
	float4 position;
	float4 color;
	float thickness;
};
RWStructuredBuffer<Line> gLines : register(u0);

cbuffer Reservoir : register(b1) {
	uint sampleCount;
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

void TangentSpace(float3 n, out float3 t, out float3 b) {
	float s = (n.z >= 0.0f) ? 1.0f : -1.0f;
	float a = -1.0f / (s + n.z);
	float bv = n.x * n.y * a;

	t = float3(1.0f + s * n.x * n.x * a, s * bv, -s * n.x);
	b = float3(bv, s + n.y * n.y * a, -n.y);
}

float3 ImportanceSampleLambert(float2 xi, float3 n) {

	float r = sqrt(xi.x);
	float phi = kTau * xi.y;

	float3 h;
	h.x = r * cos(phi);
	h.y = r * sin(phi);
	h.z = sqrt(1.0f - xi.x);

	float3 tangentX, tangentY;
	TangentSpace(n, tangentX, tangentY);

	return normalize(h.x * tangentX + h.y * tangentY + h.z * n);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	if (dispatchThreadId.x >= sampleCount) {
		return;
	}

	uint2 index = size / 2;

	float3 normal   = gNormal[index].rgb * 2.0f - 1.0f;
	float3 position = gPosition[index].rgb;

	float2 xi        = Hammersley(dispatchThreadId.x, sampleCount);
	float3 direction = ImportanceSampleLambert(xi, normal);

	for (uint i = 0; i < 2; ++i) {
		Line v;
		v.position  = float4(position + direction * i, 1.0f);
		v.color     = float4(direction.xyz, 1.0f);
		v.thickness = 0.01f;
		gLines[dispatchThreadId.x * 2 + i] = v;
	}
	
}
