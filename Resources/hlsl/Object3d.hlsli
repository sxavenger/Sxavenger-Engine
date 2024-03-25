struct VSOutput {
	float4 position : SV_POSITION;
	float4 worldPos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
};

struct GSOutput {
	float4 position : SV_POSITION;
	float4 worldPos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};