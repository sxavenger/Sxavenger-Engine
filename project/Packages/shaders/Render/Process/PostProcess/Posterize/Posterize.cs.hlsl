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

	float2 texcoord = float2(pixel + 0.5f) / dimension;

	//* coordinate posterize *//

	float2 posterized_texcoord = texcoord;

	switch (gParameter.coordinateMode) {
		case PosterizeMode::Ceil:
			posterized_texcoord = ceil(texcoord * gParameter.coordinateLevel) / gParameter.coordinateLevel;
			break;
		
		case PosterizeMode::Floor:
			posterized_texcoord = floor(texcoord * gParameter.coordinateLevel) / gParameter.coordinateLevel;
			break;
		
		case PosterizeMode::Round:
			posterized_texcoord = round(texcoord * gParameter.coordinateLevel) / gParameter.coordinateLevel;
			break;
	}

	float3 color       = gInput.SampleLevel(gSampler, posterized_texcoord, 0).rgb;
	float transparency = gInput.SampleLevel(gSampler, posterized_texcoord, 0).a;

	//* color posterize *//

	float3 posterized_color = color;

	switch (gParameter.colorMode) {
		case PosterizeMode::Ceil:
			posterized_color = ceil(color * gParameter.colorLevel) / gParameter.colorLevel;
			break;
		
		case PosterizeMode::Floor:
			posterized_color = floor(color * gParameter.colorLevel) / gParameter.colorLevel;
			break;
		
		case PosterizeMode::Round:
			posterized_color = round(color * gParameter.colorLevel) / gParameter.colorLevel;
			break;
	}

	//!< 明るさ補正
	posterized_color = posterized_color + gParameter.colorBrightnessMin * color;

	gOutput[pixel].rgb = posterized_color;
	gOutput[pixel].a   = transparency;

}
