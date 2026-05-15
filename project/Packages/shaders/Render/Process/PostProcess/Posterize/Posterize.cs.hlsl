//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Posterize.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gInput    : register(t0);
RWTexture2D<float4> gOutput : register(u0);

ConstantBuffer<Posterize> gParameter : register(b0);

SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUM_THREADS_X, NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 pixel = dispatchThreadId.xy;
	
	if (CheckOverDimension(pixel)) {
		return; //!< texture size over
	}

	//* coordinate posterize *//

	float2 texcoord = float2(pixel + 0.5f) / dimension;

	switch (gParameter.coordinateMode) {
		case PosterizeMode::Ceil:
			texcoord = ceil(texcoord * gParameter.coordinateLevel) / gParameter.coordinateLevel;
			break;
		
		case PosterizeMode::Floor:
			texcoord = floor(texcoord * gParameter.coordinateLevel) / gParameter.coordinateLevel;
			break;
		
		case PosterizeMode::Round:
			texcoord = round(texcoord * gParameter.coordinateLevel) / gParameter.coordinateLevel;
			break;
	}

	float3 color       = gInput.SampleLevel(gSampler, texcoord, 0).rgb;
	float transparency = gInput.SampleLevel(gSampler, texcoord, 0).a;

	//* color posterize *//

	switch (gParameter.colorMode) {
		case PosterizeMode::Ceil:
			color = ceil(color * gParameter.colorLevel) / gParameter.colorLevel;
			break;
		
		case PosterizeMode::Floor:
			color = floor(color * gParameter.colorLevel) / gParameter.colorLevel;
			break;
		
		case PosterizeMode::Round:
			color = round(color * gParameter.colorLevel) / gParameter.colorLevel;
			break;
	}

	gOutput[pixel].rgb = color;
	gOutput[pixel].a   = transparency;

}
