//=========================================================================================
// VSOutput
//=========================================================================================
struct VSOutput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL0;
	float3 worldPos : POSITION0;
	float2 texcoord : TEXCOORD0;
};