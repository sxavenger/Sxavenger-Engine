//=========================================================================================
// Output
//=========================================================================================
struct ParticleData {
	float4x4 mat;
	float4   color;
};
RWStructuredBuffer<ParticleData> gOutput : register(u0);

float4x4 MakeTranslate(float3 translate) {
	return float4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translate.x, translate.y, translate.z, 1.0f 
	);
}

float4x4 MakeScale(float3 scale) {
	return float4x4(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Config
////////////////////////////////////////////////////////////////////////////////////////////
#define NUMTHREAD_X 8
#define NUMTHREAD_Y 8
#define NUMTHREAD_Z 8

struct Semantic {
	uint  groupIndex       : SV_GroupIndex;
	uint3 dispatchThreadId : SV_DispatchThreadID;
	uint3 groupThreadId    : SV_GroupThreadID;
	uint3 groupId          : SV_GroupID;
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUMTHREAD_X, NUMTHREAD_Y, NUMTHREAD_Z)]
void main(Semantic semantic) {
	
	ParticleData output;
	
	uint3 dispatch = uint3(2, 2, 2);
	
	output.color
		= float4(float3(semantic.dispatchThreadId.xyz) / (8 * 2), 1.0f);
	
	//output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	output.mat
		= mul(MakeScale(float3(0.1f, 0.1f, 0.1f)), MakeTranslate(float3(semantic.dispatchThreadId.xyz)));
	
	
	// to 1coordinate thread and dispatchs
	uint id
		= semantic.dispatchThreadId.z * dispatch.x * dispatch.y * NUMTHREAD_X * NUMTHREAD_Y +
			semantic.dispatchThreadId.y * dispatch.x * NUMTHREAD_X +
			semantic.dispatchThreadId.x;
	
	gOutput[id] = output;
	
}