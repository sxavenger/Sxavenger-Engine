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
// TransformationMatrix structure buffer
//=========================================================================================
struct TransformationMatrix {
	float4x4 wvp;
	float4x4 world;
	float4x4 worldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gFloor : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(64, 1, 1)] //!< numthreads(x, y, z)の場合, x * y * z <= 128 である必要がある。
[outputtopology("triangle")]
void main(
	uint groupIndex : SV_GroupIndex,
	out vertices MSOutput verts[64],
	out indices uint3 tris[126]) {
	
	const uint vertexCount    = 4; //!< 出力する頂点数
	const uint primitiveCount = 2; //!< 三角形の数...?
	
	// スレットグループの頂点とプリミティブの数を設定
	SetMeshOutputCounts(vertexCount, primitiveCount);
	
	// 頂点番号を設定
	if (groupIndex < primitiveCount) {
		tris[groupIndex] = indices[groupIndex];
		//prims[groupIndex].Index = groupIndex;
	}
	
	// 頂点データを設定
	if (groupIndex < vertexCount) {
		
		MSOutput output;
		
		output.position = mul(vertices[groupIndex].position, gFloor.wvp);
		output.worldPos = mul(vertices[groupIndex].position, gFloor.world);
		output.texcoord = vertices[groupIndex].texcoord;
		output.normal   = normalize(mul(vertices[groupIndex].normal, (float3x3)gFloor.world));

		verts[groupIndex] = output;
		
	}

}
