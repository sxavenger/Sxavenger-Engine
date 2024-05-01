//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Floor.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// MSInput structure
////////////////////////////////////////////////////////////////////////////////////////////
struct MSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

//-----------------------------------------------------------------------------------------
// Buffers
//-----------------------------------------------------------------------------------------
StructuredBuffer<MSInput> vertices : register(t0);
StructuredBuffer<uint3> indices : register(t1);

//=========================================================================================
// FloorForGPU structure
//=========================================================================================
struct FloorForGPU {
	float4x4 wvp;
	float4x4 world;
	float4x4 worldInverseTranspose;
};
ConstantBuffer<FloorForGPU> gFloor : register(b0);

// todo: 非均一スケールの対応

////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(64, 1, 1)] //!< numthreads(x, y, z)の場合, x * y * z <= 128 である必要がある。
[outputtopology("triangle")]
void main(
	uint groupIndex : SV_GroupIndex,
	out vertices MSOutput verts[3],
	out indices uint3 tris[1]) {
	
	// スレットグループの頂点とプリミティブの数を設定
	SetMeshOutputCounts(3, 1);
	
	// 頂点番号を設定
	if (groupIndex < 1) {
		tris[groupIndex] = indices[groupIndex];
		//prims[groupIndex].Index = groupIndex;
	}
	
	// 頂点データを設定
	if (groupIndex < 3) {
		
		MSOutput output;
		
		output.position = mul(vertices[groupIndex].position, gFloor.wvp);
		output.worldPos = mul(vertices[groupIndex].position, gFloor.world);
		output.texcoord = vertices[groupIndex].texcoord;
		output.normal   = normalize(mul(vertices[groupIndex].normal, (float3x3)gFloor.world));

		verts[groupIndex] = output;
		
	}

}
