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
[maxvertexcount(6)]
void main(triangle VertexGSInput input[3], inout LineStream<PSInput> output) {

	const float distance = 0.2f;

	// 各辺を線として出力
	for (uint ti = 0; ti < 3; ++ti) {

		uint instanceId = input[ti].intanceId;
		
		for (uint vj = 0; vj < 2; ++vj) {
			
			PSInput v = (PSInput)0;

			VertexGSInput vtx = input[(ti + vj) % 3];

			float3 normal   = gTransform[instanceId].TransformNormal(vtx.normal);
			float4 position = gTransform[instanceId].Transform(float4(vtx.position.xyz + normal * distance, 1.0f));

			v.position = mul(position, viewProj);
			v.worldPos = position.xyz;
			v.normal   = normal;
			v.texcoord = vtx.texcoord;

			output.Append(v);
		}
	}
	
}