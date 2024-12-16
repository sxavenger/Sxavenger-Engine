//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Shape.hlsli"

//=========================================================================================
// Buffer
//=========================================================================================

StructuredBuffer<TransformationMatrix> gTransform : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(6 * 3)]
void main(point VertexGSInput input[1], inout TriangleStream<PSInput> output) {

	const float offset = 0.08f;
	const float distance = 0.2f;

	// 頂点のオフセット
	float3 offsets[6] = {
		float3(0.0f, offset, 0.0f), // 上
		float3(0.0f, -offset, 0.0f), // 下
		float3(-offset, 0.0f, 0.0f), // 左
		float3(offset, 0.0f, 0.0f), // 右
		float3(0.0f, 0.0f, offset), // 前
		float3(0.0f, 0.0f, -offset) // 後
	};

	// 各面を三角形で構成
	uint faces[6][3] = {
		{ 0, 2, 4 }, // 面1: 上-左-前
		{ 0, 4, 3 }, // 面2: 上-前-右
		{ 0, 3, 5 }, // 面3: 上-右-後
		{ 0, 5, 2 }, // 面4: 上-後-左
		{ 1, 4, 2 }, // 面5: 下-前-左
		{ 1, 3, 4 }  // 面6: 下-右-前
	};

	uint instanceId = input[0].intanceId;

	for (uint fi = 0; fi < 6; ++fi) {
		for (uint vj = 0; vj < 3; ++vj) {
			
			PSInput v = (PSInput)0;

			float3 normal   =  gTransform[instanceId].TransformNormal(input[0].normal);
			float4 position = gTransform[instanceId].Transform(float4(input[0].position.xyz + offsets[faces[fi][vj]] + normal * distance, 1.0f));

			v.position = mul(position, viewProj);
			v.worldPos = position.xyz;
			v.normal   = normal;
			v.texcoord = input[0].texcoord;

			output.Append(v);

		}
	}
	
}
