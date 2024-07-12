//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Matrix.hlsli"
#include "Particle.hlsli"

//=========================================================================================
// Output
//=========================================================================================
RWStructuredBuffer<ParticleData> gOutput : register(u0);
struct Flag {
	int a;
};
ConstantBuffer<Flag> gFlug : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// Config
////////////////////////////////////////////////////////////////////////////////////////////
#define NUMTHREAD_X 8
#define NUMTHREAD_Y 8
#define NUMTHREAD_Z 8

static const uint3 dispatch = uint3(2, 2, 2);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUMTHREAD_X, NUMTHREAD_Y, NUMTHREAD_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	// to 1coordinate thread and dispatchs
	uint index
		= dispatchThreadId.x
		+ dispatchThreadId.y * dispatch.x * NUMTHREAD_X
		+ dispatchThreadId.z * dispatch.x * dispatch.y * NUMTHREAD_X * NUMTHREAD_Y;
	
	ParticleData output = gOutput[index];
	
	if (!output.isUpdate || gFlug.a) {
	
		output.color
			= float4(float3(dispatchThreadId.xyz) / (8 * dispatch.xyz), 1.0f);
		
		//output.color = float4(dispatchThreadId.x / 16.0f, dispatchThreadId.x / 16.0f, dispatchThreadId.x / 16.0f, 1.0f);
		
		output.translate = float3(0.0f, 10.0f, 0.0f);
		output.velocity = (normalize(float3(dispatchThreadId.xyz) - ((dispatch.y * NUMTHREAD_X) / 2.0f)) + float3(0.0f, 1.0f, 0.0f)) * 8.0f;
		output.acceleration = float3(0.0f, 0.0f, 0.0f);
		output.mass = 2.0f;
		
		output.rotate = float3(dispatchThreadId.zyx) * 0.01f;
		output.addRotate = float3(dispatchThreadId.xyz) * 0.01f;
		
		output.isUpdate = true;
		
	} else {
		
		const float k = 0.4f;
		
		float3 airResistance = k * -output.velocity;
		float3 airResistanceAcceleration = airResistance / output.mass;
		output.acceleration = float3(0.0f, -9.8f, 0.0f) + airResistanceAcceleration;
		
		output.velocity += (output.acceleration / 60.0f);
		output.translate += (output.velocity / 60.0f);
		
		output.rotate += output.addRotate;
		
		if (output.translate.y < 0.0f) {
			output.translate.y = 0.0f;
			output.velocity.y *= -1.0f;
			output.addRotate *= 0.99f;
		}
		

	}
	
	
	output.mat = MakeAffine(float3(0.1f, 0.1f, 0.1f), output.rotate, output.translate);
	gOutput[index] = output;
	
}