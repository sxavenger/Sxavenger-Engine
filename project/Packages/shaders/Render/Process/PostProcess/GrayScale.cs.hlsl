//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"
#include "../../../Library/ACES.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Type namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Type {
	static const uint kGrayScale = 0,
	                  kSepia     = 1,
	                  kManual    = 2;
};

//=========================================================================================
// buffers
//=========================================================================================

//* textures
Texture2D<float4> gInput    : register(t0);
RWTexture2D<float4> gOutput : register(u0);

struct Parameter {
	uint type; //!< 0: gray scale, 1: sepia
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispathThreadId : SV_DispatchThreadID) {

	uint2 index = dispathThreadId.xy;

	if (CheckOverTexture(index.xy)) {
		return;
	}

	float4 color = gInput[index];

	switch (gParameter.type) {
		case Type::kGrayScale:
			{
				float3 v = dot(color.rgb, ACES::AP1_RGB2Y);
				color.rgb = float3(v);
			}
			break;
		
		case Type::kSepia:
			{
				const float3x3 sepia = float3x3(
					0.393f, 0.769f, 0.189f,
					0.349f, 0.686f, 0.168f,
					0.272f, 0.534f, 0.131f
				);
				color.rgb = mul(sepia, color.rgb);
			}
			break;

		// todo: manual
	}

	gOutput[index] = color;
	
}
