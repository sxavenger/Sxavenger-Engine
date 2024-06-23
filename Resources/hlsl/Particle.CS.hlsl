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

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUMTHREAD_X, NUMTHREAD_Y, NUMTHREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	ParticleData output;
	
	uint3 dispatch = uint3(2, 2, 2);
	
	output.color
		= float4(float3(dispatchThreadId.xyz) / (8 * dispatch.xyz), 1.0f);
	
	//output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	output.mat
		= mul(MakeScale(float3(0.1f, 0.1f, 0.1f)), MakeTranslate(float3(dispatchThreadId.xyz)));
	
	
	// to 1coordinate thread and dispatchs
	uint index
		= dispatchThreadId.x
		+ dispatchThreadId.y * dispatch.x * NUMTHREAD_X
		+ dispatchThreadId.z * dispatch.x * dispatch.y * NUMTHREAD_X * NUMTHREAD_Y;
	
	gOutput[index] = output;
	
}